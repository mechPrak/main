#define STEP_A PD2
#define DIR_A PD3
#define M0_A PD4
#define M1_A PD5
#define ENABLE_A PD6

#define STEP_B PB0
#define DIR_B PB1
#define M0_B PB2
#define M1_B PB3
#define ENABLE_B PB4

uint8_t ramp[100];

uint16_t rampCounter_A = 0;
uint16_t rampCompare_A = 500;
uint8_t rampPosition_A = 0;

uint8_t stepCounter_A = 0;
uint8_t stepCompare_A = 255;

uint8_t stepCounter_B = 0;
uint8_t stepCompare_B = 255;

void Motor_init() {
  DDRD |= ((1 << STEP_A) | (1 << DIR_A) | (1 << ENABLE_A));
  DDRB |= ((1 << STEP_B) | (1 << DIR_B) | (1 << ENABLE_B));
  PORTD |= (1 << DIR_A);

  for (uint16_t i = 0; i < 100; i++) {
    //ramp[i] = 255 - sqrt(i * 100);
    ramp[i] = int(650 / i);
  }

  // Normal Mode (Overflow)
  TCCR2A = 0B00000000;
  // Prescaler auf 1 -> mit 16MHz und OVF bei 255 -> 31,37KHz
  TCCR2B = 0B00000001;
  // Enable Overflow Interrupt
  TIMSK2 = 0B00000001;

  Motor_enableA();
  Motor_microStep_A(8);
  Motor_enableB();
  Motor_microStep_B(8);
}

ISR(TIMER2_OVF_vect) {
  if (stepCounter_A == stepCompare_A) {
    stepCounter_A = 0;
    PORTD ^= 1 << STEP_A;
  }
  stepCounter_A++;

  if (stepCounter_B == stepCompare_B) {
    stepCounter_B = 0;
    PORTB ^= 1 << STEP_B;
  }
  stepCounter_B++;

  if (rampCounter_A == rampCompare_A) {
    rampCounter_A = 0;

    rampPosition_A ++;
    if (rampPosition_A >= 99) {
      rampPosition_A = 99;
    }
    stepCompare_A = ramp[rampPosition_A];
    stepCompare_B = ramp[rampPosition_A];
  }
  rampCounter_A++;
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
