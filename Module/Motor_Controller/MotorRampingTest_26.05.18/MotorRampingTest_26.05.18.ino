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

float v0 = 0;
float v = 10000;
float a = 100000;
float F = 160000;

float S = (pow(v, 2) - pow(v0, 2)) / (2 * a);
float p1 = F / pow(pow(v0, 2) + 2 * a, 0.5);
float ps = F / v;
float R = a / pow(F, 2);
float p = p1 * ( 1 + (-R) * p1 * p1);

int cou = 0;
int del_cou = 0;

void setup() {
  DDRD |= ((1 << STEP_A) | (1 << DIR_A) | (1 << ENABLE_A));

  TCCR2A = 0b00000010;
  TCCR2B = 0b00000001;
  TIMSK2 = 0b00000010;
  OCR2A = 100;

  Motor_enableA();
  Motor_microStep_A(8);

  sei();
}

void loop() {
  
  delay(5000);

  delay(5000);
}

ISR(TIMER2_COMPA_vect) {
  if (del_cou > p) {
    del_cou = 0;
    PORTD |= (1 << STEP_A);
    if (cou < S) {
      p = p * ( 1 + (-R) * p * p);
      cou++;
    }
    PORTD &= ~(1 << STEP_A);
  }
  del_cou ++;
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
