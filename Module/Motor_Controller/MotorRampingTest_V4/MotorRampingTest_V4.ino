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

float timer2_frequenz;

enum {ACCEL, DECEL, CONST, CW, CCW};

volatile uint8_t motorA_speedChange;
volatile uint8_t motorB_speedChange;

volatile float motorA_acc;
volatile float motorA_r;
volatile float motorA_vBase;
volatile float motorA_vTarget;
volatile float motorA_steps;
volatile float motorA_del_init;
volatile float motorA_del_current;
volatile int motorA_step_counter = 0;
volatile int motorA_del_counter = 0;

volatile float motorB_acc;
volatile float motorB_r;
volatile float motorB_vBase;
volatile float motorB_vTarget;
volatile float motorB_steps;
volatile float motorB_del_init;
volatile float motorB_del_current;
volatile int motorB_step_counter = 0;
volatile int motorB_del_counter = 0;

uint32_t s = 100;

void setup() {
  motor_init();

  motor_enableA();
  motor_microStep_A(8);


  motor_enableB();
  motor_microStep_B(8);

  motorA_acceleration(500000);
  motorB_acceleration(500000);

  sei();
}

void motor_init() {
  DDRD |= ((1 << STEP_A) | (1 << DIR_A) | (1 << ENABLE_A));
  DDRB |= ((1 << STEP_B) | (1 << DIR_B) | (1 << ENABLE_B));
  PORTD |= (1 << DIR_A);

  TCCR2A = 0b00000010;
  TCCR2B = 0b00000001;
  TIMSK2 = 0b00000010;
  OCR2A = 100;

  timer2_frequenz = 160000;
  motorA_speedChange = CONST;
  motorB_speedChange = CONST;
}

void loop() {
  motorA_direction(CW);
  motorA_speed(20000);
  motorB_direction(CW);
  motorB_speed(20000);
  delay(500);
  motorA_speed(0);
  motorB_speed(0);
  delay(500);
  motorA_direction(CCW);
  motorA_speed(20000);
  motorB_direction(CW);
  motorB_speed(20000);
  delay(250);
  motorA_speed(0);
  motorB_speed(0);
  delay(500);
}

void motorA_direction(uint8_t dir) {
  if (dir == CW) {
    PORTD |= 1 << DIR_A;
  } else {
    PORTD &= ~(1 << DIR_A);
  }
}

void motorB_direction(uint8_t dir) {
if (dir == CCW) {
    PORTB |= 1 << DIR_B;
  } else {
    PORTB &= ~(1 << DIR_B);
  }
}

void motorA_speed(uint16_t target) {
  cli();
  motorA_vBase = motorA_vTarget;
  motorA_vTarget = target;
  if (motorA_vTarget > motorA_vBase) {
    motorA_speedChange = ACCEL;
  } else if (motorA_vTarget < motorA_vBase) {
    motorA_speedChange = DECEL;
  } else {
    motorA_speedChange = CONST;
  }
  if (motorA_speedChange != CONST) {
    motorA_r = motorA_acc / (timer2_frequenz * timer2_frequenz);
    motorA_del_init = timer2_frequenz / pow((motorA_vBase * motorA_vBase) + 2 * motorA_acc, 0.5);
    if (motorA_speedChange == ACCEL) {
      motorA_del_current = motorA_del_init * ( 1 + (-motorA_r) * motorA_del_init * motorA_del_init);
      motorA_steps = (pow(motorA_vTarget, 2) - pow(motorA_vBase, 2)) / (2 * motorA_acc);
    } else if (motorA_speedChange == DECEL) {
      motorA_del_current = motorA_del_init * ( 1 + (motorA_r) * motorA_del_init * motorA_del_init);
      motorA_steps = (pow(motorA_vBase, 2) - pow(motorA_vTarget, 2)) / (2 * motorA_acc);
    }
  }
  motorA_step_counter = 0;
  sei();
}

void motorB_speed(uint16_t target) {
  cli();
  motorB_vBase = motorB_vTarget;
  motorB_vTarget = target;
  if (motorB_vTarget > motorB_vBase) {
    motorB_speedChange = ACCEL;
  } else if (motorB_vTarget < motorB_vBase) {
    motorB_speedChange = DECEL;
  } else {
    motorB_speedChange = CONST;
  }
  if (motorB_speedChange != CONST) {
    motorB_r = motorB_acc / pow(timer2_frequenz, 2);
    motorB_del_init = timer2_frequenz / pow(pow(motorB_vBase, 2) + 2 * motorB_acc, 0.5);
    if (motorB_speedChange == ACCEL) {
      motorB_del_current = motorB_del_init * ( 1 + (-motorB_r) * motorB_del_init * motorB_del_init);
      motorB_steps = (pow(motorB_vTarget, 2) - pow(motorB_vBase, 2)) / (2 * motorB_acc);
    } else if (motorB_speedChange == DECEL) {
      motorB_del_current = motorB_del_init * ( 1 + (motorB_r) * motorB_del_init * motorB_del_init);
      motorB_steps = (pow(motorB_vBase, 2) - pow(motorB_vTarget, 2)) / (2 * motorB_acc);
    }
  }
  motorB_step_counter = 0;
  sei();
}

void motorA_acceleration(float acc) {
  motorA_acc = acc;
}

void motorB_acceleration(float acc) {
  motorB_acc = acc;
}

ISR(TIMER2_COMPA_vect) {
  if (motorA_del_counter > motorA_del_current) {
    motorA_del_counter = 0;

    if (motorA_vTarget == 0 && motorA_speedChange == CONST) {
    } else {
      PORTD |= (1 << STEP_A);
    }

    if (motorA_step_counter < motorA_steps) {
      if (motorA_speedChange == ACCEL) {
        motorA_del_current = motorA_del_current * ( 1 + (-motorA_r) * motorA_del_current * motorA_del_current);
      } else if (motorA_speedChange == DECEL) {
        motorA_del_current = motorA_del_current * ( 1 + (motorA_r) * motorA_del_current * motorA_del_current);
      }
      motorA_step_counter++;
    } else {
      motorA_speedChange = CONST;
    }

    PORTD &= ~(1 << STEP_A);

  }
  motorA_del_counter ++;

  if (motorB_del_counter > motorB_del_current) {
    motorB_del_counter = 0;

    if (motorB_vTarget == 0 && motorB_speedChange == CONST) {
    } else {
      PORTB |= (1 << STEP_B);
    }

    if (motorB_step_counter < motorB_steps) {
      if (motorB_speedChange == ACCEL) {
        motorB_del_current = motorB_del_current * ( 1 + (-motorB_r) * motorB_del_current * motorB_del_current);
      } else if (motorB_speedChange == DECEL) {
        motorB_del_current = motorB_del_current * ( 1 + (motorB_r) * motorB_del_current * motorB_del_current);
      }
      motorB_step_counter++;
    } else {
      motorB_speedChange = CONST;
    }

    PORTB &= ~(1 << STEP_B);

  }
  motorB_del_counter ++;

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

