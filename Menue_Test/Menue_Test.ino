#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10

#define BUTTON_1 3
#define BUTTON_2 6
#define BUTTON_3 2
#define BUTTON_4 4
#define BUTTON_5 5

#define BUTTON_TEST

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);
  pinMode(BUTTON_5, INPUT);

  tft.begin();
  tft.fillScreen(ILI9341_BLACK);

#if defined(BUTTON_TEST)
  menue_ButtonTest_init();
#endif
}

void loop() {
#if defined(BUTTON_TEST)
  menue_ButtonTest();
#endif
}
