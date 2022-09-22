#include "SPI.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Map and tiles constants
#define MAP_ROWS 6
#define MAP_COLS 16
#define TILE_SIZE 8

#define MAX_TREASURES 2
#define MAX_ENEMIES 5
// MAX_NPCS = 1
#define MAX_TRAPS 3

// Inventory and items constants
#define INVENTORY_SIZE 4
#define ITEM_SIZE 16
#define ITEM_IMAGE_SIZE 12
#define PEARLS_LIMIT 50

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// hardware pin numbers for the joystick shield V1.a
// Joystick
const uint8_t x_joystick = A0;
const uint8_t y_joystick = A1;

// Making arrays for more comfortable handling buttons
uint8_t buttons[] = {
  2, 3, 4, 5, 6, 7, 8
};

const char * key_names[] = { "A", "B", "C", "D", "E", "F", "J"};
uint8_t key_values[] = {0, 0, 0, 0, 0, 0, 0};

// Default borders for the map
const uint8_t defaultRoom[MAP_ROWS][MAP_COLS] = {
  { 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 }
};
// Map with enemies, treasures, traps and etc
uint8_t currentRoomFilling[MAP_ROWS-2][MAP_COLS-2] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

// Declaring images
static const unsigned char PROGMEM player_bmp[] = 
{
  0x00, 0x7e, 0x5a, 0x5a, 
  0x7e, 0x3c, 0x7e, 0xdb
};

static const unsigned char PROGMEM vase[] = 
{
  0x00, 0x7e, 0x18, 0x3c, 
  0x7e, 0x7e, 0x7e, 0x3c
};

static const unsigned char PROGMEM jap[] = 
{
  0x38, 0x7c, 0x38, 0x38, 
  0x5c, 0x7c, 0x28, 0x28
};

static const unsigned char PROGMEM spikes[] =
{
  0x00, 0x00, 0x00, 0x00, 
  0x04, 0x44, 0xd6, 0xff 
};

static const unsigned char PROGMEM fish[] =
{
  0x18, 0x34, 0x7e, 0x7e, 
  0xfe, 0x3c, 0x18, 0x3c
};

static const unsigned char PROGMEM shark_fangs_image[] =
{
  0x01, 0xc0, 0x03, 0xc0, 0x07, 0x80, 0x0f, 0x00, 0x0e, 0x00, 0x1e, 0x00,
  0x1f, 0x00, 0x1f, 0xc0, 0x1f, 0xc0, 0x0f, 0xc0, 0x0f, 0xc0, 0x07, 0xc0, 
};

// Declaring everything with player

// struct Spell {
//   int manaCost;
//   int effect;
//   int strength;
// };

struct Item {
  uint8_t type = 0;
  int strength;
  char description[16];
  int image_id = -1;
};

// struct Armor {
//   int defence;
//   int effectType;
// };

// Declaring player

struct Player {
  int hp, mp, pearls; // Health, mana and pearls (money)
  int ultra; // Max 5 point, when max you can use ultra skill
  int x, y; // Current position
  
  bool isOpenedInventory;
  int currentPointer;
  Item *inventory[INVENTORY_SIZE];
};

const unsigned char *items_images[]= { shark_fangs_image, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

Player player;

Item shark_fangs;
Item empty_item;

void setup() {
  Serial.begin(9600);
  // Init and clear
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  delay(2000);
  display.clearDisplay();
 
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  
  // Drawing intro
  display.println("Sirop 2022");
  display.display(); 
  
  delay(2000);

  // Clearing display
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setCursor(0, 0);

  // Add buttons to input
  for (int i; i < 7; i++) {
    pinMode(buttons[i], INPUT);
  }

  // First init map
  // generateRoomFillings();
  // updateMap();

  // Init player
  player.x = 5;
  player.y = 2;

  player.hp = 10;
  player.mp = 10;
  player.pearls = 5;
  player.ultra = 5;

  player.currentPointer = 0;
  player.isOpenedInventory = false;

  // Init items
  shark_fangs.image_id = 0;
  shark_fangs.strength = 8;
  shark_fangs.type = 1;

  empty_item.image_id = 4;
  empty_item.type = 0;

  player.inventory[0] = &shark_fangs;

  for (int i = 1; i < INVENTORY_SIZE; i++) {
    player.inventory[i] = &empty_item;
  }

  // First generation of map
  updateMap();

}

// Generate random number in [min; max]
uint8_t randInt(uint8_t min, uint8_t max) {
  return (uint8_t) (millis() * analogRead(0) + analogRead(A4) + (analogRead(A0) * analogRead(A1))) % (max + 1) + min; 
}

void updateMap() {
  int current_enemies = randInt(0, MAX_ENEMIES);
  int x, y;
  int current_treasures;
  int current_traps; 
  int current_npcs;

  for (int row = 0; row < MAP_ROWS - 2; row++) {
      for (int col = 0; col < MAP_COLS - 2; col++) {
        currentRoomFilling[row][col] = 0;
      }
  }

  while (current_enemies > 0) {
    x = randInt(0, MAP_COLS-2);
    y = randInt(0, MAP_ROWS-2);
    currentRoomFilling[y][x] = 11;
    current_enemies--; 
    Serial.print(y); Serial.print(' '); Serial.print(x); Serial.print("\n");
    
    
  }
  int r = randInt(0, 3);
  switch (r) {
    case 0:
      current_traps = randInt(0, MAX_TRAPS);
      while (current_traps > 0) {
        x = randInt(0, MAP_COLS-2);
        y = randInt(0, MAP_ROWS-2);
        currentRoomFilling[y][x] = 21;
        current_traps--; 
      }
      break;
    case 1:
      current_treasures = randInt(0, MAX_TREASURES);
      while (current_treasures > 0) {
        x = randInt(0, MAP_COLS-2);
        y = randInt(0, MAP_ROWS-2);
        currentRoomFilling[y][x] = 31;
        current_treasures--; 
      }
      break;
    case 2:
      current_npcs = 1;
      x = randInt(0, MAP_COLS-2);
      y = randInt(0, MAP_ROWS-2);
      currentRoomFilling[y][x] = 41;
      break;
  }
  
}

void drawRoomFillings() {
  for (uint8_t row = 0; row < MAP_ROWS - 2; row++) {
    for (uint8_t col = 0; col < MAP_COLS - 2; col++) {
      uint8_t filling = currentRoomFilling[row][col];
      switch (filling) {
        case 11:
          display.drawBitmap(col*8+8, row*8+40, jap, TILE_SIZE, TILE_SIZE, 1);
          break;
        case 21:
          display.drawBitmap(col*8+8, row*8+40, spikes, TILE_SIZE, TILE_SIZE, 1);
          break;
        case 31:
          display.drawBitmap(col*8+8, row*8+40, vase, TILE_SIZE, TILE_SIZE, 1);
          break;
        case 41:
          display.drawBitmap(col*8+8, row*8+40, fish, TILE_SIZE, TILE_SIZE, 1);
          break;
      }
    }
  }
  
}

// Making flash screen effect
void changeRoom() {
  for (uint8_t e = 1; e < 11; e++) {
    display.invertDisplay(e%2);
    delay(150);
  }
  updateMap();
}

// Handling buttons
void handle_button(const char *button_name, bool new_val, uint8_t button_num) {
  if (new_val) {
    key_values[button_num] = 1;
  }
}

// Handling joystick
void handle_joystick() {
  int val_x_joystick = map(analogRead(x_joystick), 1000, 0, 1, -1);
  int val_y_joystick = map(analogRead(y_joystick), 1000, 0, -1, 1);

  if (!player.isOpenedInventory) {
    if (defaultRoom[player.y + val_y_joystick][player.x + val_x_joystick] == 0) {
      player.x += val_x_joystick;
      player.y += val_y_joystick;
      if (player.x > 15) {
        changeRoom();
        player.x = 0;
      }
      if (player.x < 0) {
        changeRoom();
        player.x = 15;
      } 
    } else if (player.y + val_y_joystick < 0) {
      changeRoom();
      player.y = 5;
    } else if (player.y + val_y_joystick > 5) {
      changeRoom();
      player.y = 0;
    }
  } else {
    if (!(player.currentPointer + val_x_joystick < 0 || player.currentPointer + val_x_joystick > 3)) {
      player.currentPointer += val_x_joystick;
    }
  }
  
}

// Draw graphics for each call
void drawMap() {
  for (uint8_t row = 0; row < MAP_ROWS; row++) {
    for (uint8_t col = 0; col < MAP_COLS; col++) {
      if (defaultRoom[row][col] == 1) {
        display.fillRect(col*8, row*8 + TILE_SIZE*2, TILE_SIZE, TILE_SIZE, SSD1306_WHITE); 
      }
    }
    display.drawBitmap(player.x*8, player.y*8 + TILE_SIZE*2, player_bmp, TILE_SIZE, TILE_SIZE, 1);
  }
}

// Drawing inventory
void drawInventory() {
  for (uint8_t i = 0; i < INVENTORY_SIZE; i++) {
    if (i != player.currentPointer) {
      display.drawRect(i*16+32, 24, ITEM_SIZE, ITEM_SIZE, SSD1306_WHITE);
    } else {
      uint8_t coord = i*16 + 32;
      display.drawRect(coord, 24, ITEM_SIZE, ITEM_SIZE, SSD1306_WHITE);
      display.drawRect(coord+1, 24+1, ITEM_SIZE-1, ITEM_SIZE-1, SSD1306_WHITE);
      display.drawRect(coord+2, 24+2, ITEM_SIZE-2, ITEM_SIZE-2, SSD1306_WHITE);
    }
    if (player.inventory[i]->type != 0) {
      display.drawBitmap(i*16+32+2, 24+2, items_images[player.inventory[i]->image_id], ITEM_IMAGE_SIZE, ITEM_IMAGE_SIZE, 1);
    }
    
  }
}

void drawGUI() {
  display.setCursor(0, 0);
  // Health points
  display.print("HP:"); display.print(player.hp);
  // Pearls
  display.print(" P:"); display.print(player.pearls);
  // Ultra
  display.print(" U:");
  for (uint8_t u = 0; u < player.ultra; u++) {
    display.print("|");
  }

  // Mana points
  display.print("\nMP:"); display.print(player.mp);
  if (player.isOpenedInventory) {
    display.print("\nInventory");
  }
}

// Updating everything for loop
void update() {
  
  display.display();
  delay(100);

  display.clearDisplay();
  display.display();
  display.setCursor(0, 0);
  
  
  for (uint8_t i = 0; i < 7; i++) {
    key_values[i] = 0;
  }
}

void loop() {
  for (uint8_t i = 0; i < 7; i++) {
    handle_button(key_names[i], digitalRead(buttons[i]) == LOW, i);
  }

  if (key_values[4] == 1 && player.isOpenedInventory) {
    player.isOpenedInventory = false;
  } else if (key_values[4] == 1 && !player.isOpenedInventory) {
    player.isOpenedInventory = true;
  }
  drawGUI();
  if (!player.isOpenedInventory) {
    
    drawMap();
    drawRoomFillings();
  } else {
    drawInventory();
  }
  handle_joystick();
  
  update();
}

void yield() {
  // Code for doing while delay is called
  
}
