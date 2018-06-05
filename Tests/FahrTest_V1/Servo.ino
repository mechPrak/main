void initServo() {
  // Timer1
  // CTC Mode | Prescaler
  TCCR1B |= ((1 << WGM12) | (1 << CS10));
  // output compare interrupt enable
  TIMSK1 |= (1 << OCIE1A);

  OCR1AH = 0b00000000;
  OCR1AL = 0b00000000;

//  TCCR1A = 0b00000000;
//  TCCR1B = 0b00001010;
//  TCCR1C = 0b00000000;
//  TIMSK1 = 0b00000010;
//
//  OCR1AH = 0b00000000;
//  OCR1AL = 0b00000010;

  DDRB |= (1 << PB5);
}

ISR(TIMER1_COMPA_vect) {
  PORTB |= (1 << PB5);
  PORTB &= ~(1 << PB5);
}
