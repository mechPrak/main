// Configuration der Motor Pins sowie Timer0 und Timer2
void initMotor() {
  DDRD |= ((1 << STEP_A) | (1 << DIR_A) | (1 << ENABLE_A));
  DDRB |= ((1 << STEP_B) | (1 << DIR_B) | (1 << ENABLE_B));

  // Timer 0
  // CTC Mode
  TCCR0A = B00000010;
  // Prescaler 64
  TCCR0B = B00000011;
  // Output Compare A Match Interrupt Enable
  TIMSK0 = B00000010;

  // Timer 2
  // CTC Mode
  TCCR2A = B00000010;
  // Prescaler 64
  TCCR2B = B00000100;
  // Output Compare A Match Interrupt Enable
  TIMSK2 = B00000010;
}

// Setzt MicroStepping für MotorA
// Werte: 1,2,4,8,16,32
void Motor_microStep_A(uint8_t stepSize) {
  switch (stepSize) {
    case 1:
      DDRD |= (1 << M0_A);
      DDRD |= (1 << M1_A);
      PORTD &= ~(1 << M0_A);
      PORTD &= ~(1 << M1_A);
      break;
    case 2:
      DDRD |= (1 << M0_A);
      DDRD |= (1 << M1_A);
      PORTD |= (1 << M0_A);
      PORTD &= ~(1 << M1_A);
      break;
    case 4:
      DDRD &= ~(1 << M0_A);
      DDRD |= (1 << M1_A);
      PORTD &= ~(1 << M0_A);
      PORTD &= ~(1 << M1_A);
      break;
    case 8:
      DDRD |= (1 << M0_A);
      DDRD |= (1 << M1_A);
      PORTD &= ~(1 << M0_A);
      PORTD |= (1 << M1_A);
      break;
    case 16:
      DDRD |= (1 << M0_A);
      DDRD |= (1 << M1_A);
      PORTD |= (1 << M0_A);
      PORTD |= (1 << M1_A);
      break;
    case 32:
      DDRD &= ~(1 << M0_A);
      DDRD |= (1 << M1_A);
      PORTD &= ~(1 << M0_A);
      PORTD |= (1 << M1_A);
      break;
  }
}

// Setzt MicroStepping für MotorB
// Werte: 1,2,4,8,16,32
void Motor_microStep_B(uint8_t stepSize) {
  switch (stepSize) {
    case 1:
      DDRB |= (1 << M0_B);
      DDRB |= (1 << M1_B);
      PORTB &= ~(1 << M0_B);
      PORTB &= ~(1 << M1_B);
      break;
    case 2:
      DDRB |= (1 << M0_B);
      DDRB |= (1 << M1_B);
      PORTB |= (1 << M0_B);
      PORTB &= ~(1 << M1_B);
      break;
    case 4:
      DDRB &= ~(1 << M0_B);
      DDRB |= (1 << M1_B);
      PORTB &= ~(1 << M0_B);
      PORTB &= ~(1 << M1_B);
      break;
    case 8:
      DDRB |= (1 << M0_B);
      DDRB |= (1 << M1_B);
      PORTB &= ~(1 << M0_B);
      PORTB |= (1 << M1_B);
      break;
    case 16:
      DDRB |= (1 << M0_B);
      DDRB |= (1 << M1_B);
      PORTB |= (1 << M0_B);
      PORTB |= (1 << M1_B);
      break;
    case 32:
      DDRB &= ~(1 << M0_B);
      DDRB |= (1 << M1_B);
      PORTB &= ~(1 << M0_B);
      PORTB |= (1 << M1_B);
      break;
  }
}

void Motor_enableA() {
  PORTD |= (1 << ENABLE_A);
}

void Motor_enableB() {
  PORTB |= (1 << ENABLE_B);
}

void Motor_disableA() {
  PORTD &= ~(1 << ENABLE_A);
}

void Motor_disableB() {
  PORTB &= ~(1 << ENABLE_B);
}

ISR(TIMER0_COMPA_vect) {
  if (stepCounter_A == stepDivider_A) {
    stepCounter_A = 0;
    PORTD ^= (1 << STEP_A);
  }
  stepCounter_A++;
}

ISR(TIMER2_COMPA_vect) {
  if (stepCounter_B == stepDivider_B) {
    stepCounter_B = 0;
    PORTB ^= (1 << STEP_B);
  }
  stepCounter_B++;
}

