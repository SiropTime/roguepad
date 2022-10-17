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
#define INVENTORY_SIZE 5
#define ITEM_HEIGHT 10
// For item width use SCREEN_WIDTH
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


// Declaring everything with player
struct Item {
  char name[8];
  uint8_t type = 0;
  uint8_t strength;
  char description[16];
};

// Declaring player

struct Player {
  uint8_t hp, mp, pearls; // Health, mana and pearls (money)
  uint8_t ultra; // Max 5 point, when max you can use ultra skill
  uint8_t x, y; // Current position
  
  char playerSymbol = '@';

  bool isOpenedInventory;
  uint8_t currentPointer;
  Item *inventory[INVENTORY_SIZE];

  bool checkBorders(uint8_t nx, uint8_t ny) {
    if ((nx < 1 || nx > 14) && !(ny > 1 && ny < 4)) return true;
    if ((ny < 1 || ny > 4) && !(nx > 6 && nx < 9)) return true;

    return false;
  }
};


Player player;

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

  // Init player
  player.x = 5;
  player.y = 2;

  player.hp = 10;
  player.mp = 10;
  player.pearls = 5;
  player.ultra = 5;

  player.currentPointer = 0;
  player.isOpenedInventory = false;

}

// Generate random number in [min; max]
uint8_t randInt(uint8_t min, uint8_t max) {
  return (uint8_t) (millis() * analogRead(0) + analogRead(A4) + (analogRead(A0) * analogRead(A1))) % (max + 1) + min; 
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

      player.x += val_x_joystick;
      player.y += val_y_joystick;

  } else {
    if (!(player.currentPointer + val_y_joystick < 0 || player.currentPointer + val_y_joystick > 5)) {
      player.currentPointer += val_y_joystick;
    }
  }
  
}

// Draw graphics for each call
void drawMap() {


  // Drawing player
  display.setCursor(player.x*TILE_SIZE, (player.y + 2)*TILE_SIZE);
  display.write(player.playerSymbol);
}

// Drawing inventory
void drawInventory() {
  for (uint8_t i = 0; i < INVENTORY_SIZE; i++) {
    if (i != player.currentPointer) {
      display.drawRect(0, i * ITEM_HEIGHT + 16, SCREEN_WIDTH, ITEM_HEIGHT, SSD1306_WHITE);
    } else {
      uint8_t coord = i*ITEM_HEIGHT + 16;
      display.drawRect(0, coord, SCREEN_WIDTH, ITEM_HEIGHT, SSD1306_WHITE);
      display.drawRect(0+1, coord+1, SCREEN_WIDTH-1, ITEM_HEIGHT-1, SSD1306_WHITE);
      display.drawRect(0+2, coord+2, SCREEN_WIDTH-2, ITEM_HEIGHT-2, SSD1306_WHITE);
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
  } else {
    drawInventory();
  }
  handle_joystick();
  
  update();
}

void yield() {
  // Code for doing while delay is called
  
}
