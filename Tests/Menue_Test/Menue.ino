void menue_draw() {

}

void menue_curserUp() {

}

void menue_curserDown() {

}

void menue_moveIn() {

}

void menue_moveOut() {

}

void menue_ButtonTest_init() {
  tft.fillRect(95, 135, 50, 50, ILI9341_LIGHTGREY);
  tft.fillRect(95, 60, 50, 50, ILI9341_LIGHTGREY);
  tft.fillRect(95, 210, 50, 50, ILI9341_LIGHTGREY);
  tft.fillRect(20, 135, 50, 50, ILI9341_LIGHTGREY);
  tft.fillRect(170, 135, 50, 50, ILI9341_LIGHTGREY);
}

void menue_ButtonTest() {
  int fill_notPressed = ILI9341_DARKGREY;
  int fill_pressed = ILI9341_RED;
  int fill;

  if (digitalRead(BUTTON_3)) {
    fill = fill_notPressed;
  } else {
    fill = fill_pressed;
  }
  tft.fillCircle(120, 160, 15, fill);

  if (digitalRead(BUTTON_2)) {
    fill = fill_notPressed;
  } else {
    fill = fill_pressed;
  }
  tft.fillCircle(120, 85, 15, fill);

  if (digitalRead(BUTTON_4)) {
    fill = fill_notPressed;
  } else {
    fill = fill_pressed;
  }
  tft.fillCircle(120, 235, 15, fill);

  if (digitalRead(BUTTON_1)) {
    fill = fill_notPressed;
  } else {
    fill = fill_pressed;
  }
  tft.fillCircle(45, 160, 15, fill);

  if (digitalRead(BUTTON_5)) {
    fill = fill_notPressed;
  } else {
    fill = fill_pressed;
  }
  tft.fillCircle(195, 160, 15, fill);
}

