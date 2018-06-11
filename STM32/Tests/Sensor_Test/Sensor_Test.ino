void setup() {
  pinMode(PC13, OUTPUT);
  pinMode(PA0, INPUT);
}

void loop() {
  while (1) {
    GPIOC_BASE -> BSRR = 0b1 << 13;
    analogRead(PA0);
    GPIOC_BASE -> BSRR = 0b1 << 29;
  }
}
