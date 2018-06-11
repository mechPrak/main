void setup() {
  digital_io_init(0, 0, 0);
  //pinMode(PC13, OUTPUT);
  //RCC_APB2ENR = 0b1 << 4;
  //GPIOC_BASE -> CRH = 0b1 << 20;
  //GPIOC_BASE -> CRH = 0b1 << 21;

  /*Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
    Timer2.setPeriod(1); // in microseconds
    Timer2.setCompare(TIMER_CH1, 1);      // overflow might be small
    Timer2.attachInterrupt(TIMER_CH1, tog);*/
}

void loop() {
  while (1) {
    GPIOC_BASE -> BSRR = 0b1 << 13;
    GPIOC_BASE -> BSRR = 0b1 << 29;
  }
}

/*
  void tog() {
  toggle ^= 1;
  digitalWrite(PC13, toggle);
  }
*/

