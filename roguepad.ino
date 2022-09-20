#include "SPI.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define MAP_ROWS 6
#define MAP_COLS 16

#define TILE_SIZE 8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// hardware pin numbers for the joystick shield V1.a
const int a_button = 2;   
const int b_button = 3;   
const int c_button = 4;
const int d_button = 5;

const int e_button = 6;
const int f_button = 7;

const int j_button = 8;
const int x_joystick = A0;
const int y_joystick = A1;

// Making arrays for more comfortable handling buttons
int buttons[] = {
  a_button, b_button, c_button, d_button, e_button, f_button, j_button};

const char * key_names[] = { "A", "B", "C", "D", "E", "F", "J"};
int key_values[] = {0, 0, 0, 0, 0, 0, 0};

// Declaring map and rooms
int m_map[MAP_ROWS][MAP_COLS];
int defaultRoom[MAP_ROWS][MAP_COLS] = {
  { 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 }
};

// Declaring character and everything with him
struct Player {
  int hp, mp, pearls; // Health, mana and pearls (money)
  int ultra; // Max 5 point, when max you can use ultra skill
  int x, y; // Current position
  bool isOpenedInventory;


};

static const unsigned char PROGMEM player_bmp[] = 
{
  0x00, 
  0x7e, 
  0x5a, 
  0x5a, 
  0x7e, 
  0x3c, 
  0x7e, 
  0xdb
};

struct Spell {
  int manaCost;
  String effect;
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

  display.println("Starting  game!");
  display.display(); 
  
  delay(1000);

  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setCursor(0, 0);

  // Add buttons to input
  for (int i; i < 7; i++) {
    pinMode(buttons[i], INPUT);
  }

  for (int row = 0; row < MAP_ROWS; row++) {
    for (int col = 0; col < MAP_COLS; col++) {
      m_map[row][col] = random(0, 2);
      Serial.println(m_map[row][col]);
    }
  }

  // Init player
  player.x = 5;
  player.y = 2;

  player.hp = 10;
  player.mp = 10;
  player.pearls = 5;
  player.ultra = 5;
  player.isOpenedInventory = false;
}

// Handling buttons and displaying them
void handle_button(const char *button_name, bool new_val, int button_num) {

  if (new_val) {
    key_values[button_num] = 1;
  }
}

// Handling joystick and displaying it
void handle_joystick() {
  int val_x_joystick = map(analogRead(x_joystick), 1000, 0, 1, -1);
  int val_y_joystick = map(analogRead(y_joystick), 1000, 0, -1, 1);
  Serial.println(val_x_joystick, val_y_joystick);

  if (defaultRoom[player.y + val_y_joystick][player.x + val_x_joystick] == 0) {
    player.x += val_x_joystick;
    player.y += val_y_joystick;
  }
  
}

// Draw graphics for each call
void drawMap() {
  for (int row = 0; row < MAP_ROWS; row++) {
    for (int col = 0; col < MAP_COLS; col++) {
      if (defaultRoom[row][col]) {
        display.fillRect(col*8, row*8 + TILE_SIZE*2, TILE_SIZE, TILE_SIZE, SSD1306_WHITE);
      }
      
    }
  }
  display.drawBitmap(player.x*8, player.y*8 + TILE_SIZE*2, player_bmp, TILE_SIZE, TILE_SIZE, 1);

}

void drawGUI() {
  display.setCursor(0, 0);
  // Health points
  display.print("HP:"); display.print(player.hp);
  // Pearls
  display.print(" P:"); display.print(player.pearls);
  // Ultra
  display.print(" U:");
  for (int u = 0; u < player.ultra; u++) {
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
  
  
  for (int i = 0; i < 7; i++) {
    key_values[i] = 0;
  }
}

void loop() {
  for (int i = 0; i < 7; i++) {
    handle_button(key_names[i], digitalRead(buttons[i]) == LOW, i);
    Serial.print(key_names[i]);
    Serial.print(key_values[i]);
    
  }

  if (key_values[4] == 1 && player.isOpenedInventory) {

  } else if (key_values[4] == 1 && !player.isOpenedInventory) {

  }

  Serial.println("");
  handle_joystick();
  drawGUI();
  if (!player.isOpenedInventory) {
    drawMap();
  }
  

  update();
  
}
