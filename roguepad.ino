#include "SPI.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define _keys_KEYLEN 4
#define _keys_VALLEN 4

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


int buttons[] = {
  a_button, b_button, c_button, d_button, e_button, f_button, j_button};

boolean prev_j_button = false;

String key_names[] = { "A", "B", "C", "D", "E", "F", "J"};
int key_values[] = {0, 0, 0, 0, 0, 0, 0};

void setup() {

  Serial.begin(9600);
// инициализация и очистка дисплея
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
 
  display.setTextSize(2); // установка размера шрифта
  display.setTextColor(WHITE); // установка цвета текста
  display.setCursor(0, 10); // установка курсора

  display.println("Starting  game!");
  display.display(); 
  delay(1000);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setCursor(0, 0);

  for (int i; i < 7; i++) {
    pinMode(buttons[i], INPUT);
  }

  
}

void handle_button(const String *button_name, bool new_val, int button_num) {

  if (new_val) {
    key_values[button_num] = 1;
  }

  display.print(key_names[button_num]);
  display.print(key_values[button_num]);
}

void update() {
  // display.clearDisplay();
  display.setCursor(0, 0);
  delay(200);
  display.display();
  for (int i = 0; i < 7; i++) {
    key_values[i] = 0;
  }
}

void loop() {
  

  for (int i = 0; i < 7; i++) {
    handle_button(&key_names[i], digitalRead(buttons[i]) == LOW, i);

    Serial.print(key_names[i]);
    Serial.print(key_values[i]);

  }

  // to parse analog sensors to digital ones, we first read them and 
  // map them to a value in [-1, 0, 1]
  // int val_x_joystick = map(analogRead(x_joystick), 0, 1000, -1, 1);
  // int val_y_joystick = map(analogRead(y_joystick), 0, 1000, -1, 1);
  update();
  
}
