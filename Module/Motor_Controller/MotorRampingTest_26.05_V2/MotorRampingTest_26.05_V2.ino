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

enum {ACCEL, DECEL, CONST};
uint8_t speed_change;

float frq;
float acc;
float r;
float v_base;
float v_target;
float steps;
float del_init;
float del_current;
volatile int step_counter = 0;
volatile int del_counter = 0;

void setup() {
  DDRD |= ((1 << STEP_A) | (1 << DIR_A) | (1 << ENABLE_A));

  TCCR2A = 0b00000010;
  TCCR2B = 0b00000001;
  TIMSK2 = 0b00000010;
  OCR2A = 100;

  motor_enableA();
  motor_microStep_A(8);

  acc = 200000;
  frq = 160000;

  del_init = frq / pow(2 * acc, 0.5);
  r = acc / pow(frq, 2);
  del_current = del_init * ( 1 + (-r) * del_init * del_init);

  speed_change = CONST;

  Serial.begin(115200);

  sei();
}

void loop() {
  /*
  delay(2000);
  cli();
  v_base = 0;
  v_target = 10000;
  speed_change = ACCEL;

  del_init = frq / pow(pow(v_base, 2) + 2 * acc, 0.5);
  del_current = del_init * ( 1 + (-r) * del_init * del_init);

  steps = (pow(v_target, 2) - pow(v_base, 2)) / (2 * acc);
  step_counter = 0;
  sei();
  */
/*
  delay(2000);
  cli();
  v_base = 10000;
  v_target = 0;
  speed_change = DECEL;

  del_init = frq / pow(pow(v_base, 2) + 2 * acc, 0.5);
  del_current = del_init * ( 1 + (r) * del_init * del_init);

  steps = (pow(v_base, 2) - pow(v_target, 2)) / (2 * acc);
  step_counter = 0;
  sei();*/
  
  
  motor_speed_A(10000);
  delay(2000);
  motor_speed_A(0);
  delay(2000);
}

void motor_speed_A(uint16_t target) {
  cli();
  v_base = v_target;
  v_target = target;
  if (v_target > v_base) {
    speed_change = ACCEL;
  } else if ( v_target < v_base) {
    speed_change = DECEL;
  } else {
    speed_change = CONST;
  }
  if (speed_change != CONST) {
    del_init = frq / pow(pow(v_base, 2) + 2 * acc, 0.5);
    if (speed_change == ACCEL) {
      del_current = del_init * ( 1 + (-r) * del_init * del_init);
      steps = (pow(v_target, 2) - pow(v_base, 2)) / (2 * acc);
    } else if (speed_change == DECEL) {
      del_current = del_init * ( 1 + (r) * del_init * del_init);
      steps = (pow(v_base, 2) - pow(v_target, 2)) / (2 * acc);
    }
  }
  step_counter = 0;

  Serial.print("target: ");
  Serial.println(target);

  Serial.print("v_base: ");
  Serial.println(v_base);

  Serial.print("speed_change: ");
  Serial.println(speed_change);

  Serial.print("acc: ");
  Serial.println(acc);

  Serial.print("r: ");
  Serial.println(r);

  Serial.print("del_init: ");
  Serial.println(del_init);

  Serial.print("steps: ");
  Serial.println(steps);
  Serial.println();
  
  sei();
}

ISR(TIMER2_COMPA_vect) {
  if (del_counter > del_current) {
    del_counter = 0;

    if (v_target == 0 && speed_change == CONST) {
    } else {
      PORTD |= (1 << STEP_A);
    }

    if (step_counter < steps) {
      if (speed_change == ACCEL) {
        del_current = del_current * ( 1 + (-r) * del_current * del_current);
      } else if (speed_change == DECEL) {
        del_current = del_current * ( 1 + (r) * del_current * del_current);
      }
      step_counter++;
    } else {
      speed_change = CONST;
    }

    PORTD &= ~(1 << STEP_A);

  }
  del_counter ++;
}

void motor_microStep_A(uint8_t stepSize) {
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

void motor_microStep_B(uint8_t stepSize) {
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

void motor_enableA() {
  PORTD |= (1 << ENABLE_A);
}

void motor_enableB() {
  PORTB |= (1 << ENABLE_B);
}

void motor_disableA() {
  PORTD &= ~(1 << ENABLE_A);
}

void motor_disableB() {
  PORTB &= ~(1 << ENABLE_B);
}

