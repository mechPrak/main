uint8_t stepA_div = 10;
uint8_t stepA_counter = 0;

uint8_t stepB_div = 10;
uint8_t stepB_counter = 0;

volatile boolean incSpeed;

void setup() {
  // STEPA
  DDRD |= (1 << PD2);
  // ENABLEA
  DDRD |= (1 << PD6);
  PORTD |= (1 << PD6);
  
  // STEPB
  DDRB |= (1 << PB0);
  // ENABLEB
  DDRB |= (1 << PB4);
  PORTB |= (1 << PB4);

  // CTC Mode
  TCCR0A = B00000010;
  // Prescaler 64
  TCCR0B = B00000011;
  // Output Compare A Match Interrupt Enable
  TIMSK0 = B00000010;


  // CTC Mode
  TCCR2A = B00000010;
  // Prescaler 64
  TCCR2B = B00000100;
  // Output Compare A Match Interrupt Enable
  TIMSK2 = B00000010;


  OCR0A = 5;
  OCR2A = 50;
  
  sei();
}

void loop() {
  delay(250);
  incSpeed = true;
}

ISR(TIMER0_COMPA_vect) {
  if (stepA_counter == stepA_div) {
    stepA_counter = 0;
    PORTD ^= (1 << PD2);
    //if(incSpeed){
      //incSpeed = false;
      //OCR0A ++;
    //}
  }
  stepA_counter++;
}

ISR(TIMER2_COMPA_vect) {
  if (stepB_counter == stepB_div) {
    stepB_counter = 0;
    //PORTB ^= (1 << PB0);
  }
  stepB_counter++;
}
