float mc_timerFrequenz;

// Motor A
#define DDR_A DDRB
#define PORT_A PORTB
#define STEP_A PB2
#define DIR_A PB1
#define M0_A PD4
#define M1_A PD5
#define ENABLE_A PD6
float mc_motorA_vBase;
float mc_motorA_vTarget;
float mc_motorA_acc;
float mc_motorA_del_init;
volatile uint8_t mc_motorA_acceleration;
volatile float mc_motorA_r;
volatile float mc_motorA_steps;
volatile float mc_motorA_del_current;
volatile uint16_t mc_motorA_step_counter;
volatile uint16_t mc_motorA_del_counter;
volatile uint16_t mc_motorA_stepCount;


// Motor B
#define DDR_B DDRB
#define PORT_B PORTB
#define STEP_B PB4
#define DIR_B PB3
#define M0_B PB2
#define M1_B PB3
#define ENABLE_B PB4
float mc_motorB_vBase;
float mc_motorB_vTarget;
float mc_motorB_acc;
float mc_motorB_del_init;
volatile uint8_t mc_motorB_acceleration;
volatile float mc_motorB_r;
volatile float mc_motorB_steps;
volatile float mc_motorB_del_current;
volatile uint16_t mc_motorB_step_counter = 0;
volatile uint16_t mc_motorB_del_counter = 0;
volatile uint16_t mc_motorB_stepCount;

ISR(TIMER2_COMPA_vect) {
  //check if del_counter is high enough, higher than del_current, for the next step
  if (mc_motorA_del_counter > mc_motorA_del_current) {
    //reset del_counter to zero
    mc_motorA_del_counter = 0;
    //check if target velocity is zero and motor acceleration is constant
    if (mc_motorA_vTarget == 0 && mc_motorA_acceleration == CONST) {
      //do nothing so the motor stands still
    } else {
      //pull step pin high for the next step
      PORT_A |= (1 << STEP_A);
      mc_motorA_stepCount++;
    }
    //check if step_counter (already made steps) is lower than steps (total steps to make)
    if (mc_motorA_step_counter < mc_motorA_steps) {
      //check if motor acceleration is accelerating ore decelerating and then calculate the next step delay
      if (mc_motorA_acceleration == ACCEL) {
        mc_motorA_del_current = mc_motorA_del_current * ( 1 + (-mc_motorA_r) * mc_motorA_del_current * mc_motorA_del_current);
      } else if (mc_motorA_acceleration == DECEL) {
        mc_motorA_del_current = mc_motorA_del_current * ( 1 + (mc_motorA_r) * mc_motorA_del_current * mc_motorA_del_current);
      }
      //increase step counter
      mc_motorA_step_counter++;
    } else {
      //if step counter is lower, then set acceleration to constant
      mc_motorA_acceleration = CONST;
    }
    //pull step pin low
    PORT_A &= ~(1 << STEP_A);
  }
  //increase delay counter
  mc_motorA_del_counter ++;


  if (mc_motorB_del_counter > mc_motorB_del_current) {
    mc_motorB_del_counter = 0;
    if (mc_motorB_vTarget == 0 && mc_motorB_acceleration == CONST) {
    } else {
      PORT_B |= (1 << STEP_B);
      mc_motorB_stepCount++;
    }
    if (mc_motorB_step_counter < mc_motorB_steps) {
      if (mc_motorB_acceleration == ACCEL) {
        mc_motorB_del_current = mc_motorB_del_current * ( 1 + (-mc_motorB_r) * mc_motorB_del_current * mc_motorB_del_current);
      } else if (mc_motorB_acceleration == DECEL) {
        mc_motorB_del_current = mc_motorB_del_current * ( 1 + (mc_motorB_r) * mc_motorB_del_current * mc_motorB_del_current);
      }
      mc_motorB_step_counter++;
    } else {
      mc_motorB_acceleration = CONST;
    }
    PORTB &= ~(1 << STEP_B);
  }
  mc_motorB_del_counter ++;
}

void motor_init() {
  //set pins as output
  DDR_A |= ((1 << STEP_A) | (1 << DIR_A) | (1 << ENABLE_A));
  DDR_B |= ((1 << STEP_B) | (1 << DIR_B) | (1 << ENABLE_B));

  //configure timer 2
  TCCR2A = 0b00000010;
  //prescaling 8
  TCCR2B = 0b00000010;
  TIMSK2 = 0b00000010;
  OCR2A = 100;

  //set frequency to 160KHz
  mc_timerFrequenz = 20000;

  //initial direction and acceleration of both motors
  motorA_direction(CCW);
  motorB_direction(CCW);
  mc_motorA_acceleration = CONST;
  mc_motorB_acceleration = CONST;
}

void motorA_direction(uint8_t dir) {
  if (dir == CW) {
    PORT_A |= 1 << DIR_A;
  } else {
    PORT_A &= ~(1 << DIR_A);
  }
}

void motorB_direction(uint8_t dir) {
  if (dir == CCW) {
    PORT_B |= 1 << DIR_B;
  } else {
    PORT_B &= ~(1 << DIR_B);
  }
}

void motorA_speed(uint16_t target) {
  //disable interrupts
  cli();
  //set base velocity to last target velocity
  mc_motorA_vBase = mc_motorA_vTarget;
  //set target velocity to new velocity
  mc_motorA_vTarget = target;
  //check is target velocity is higher than base velocity
  if (mc_motorA_vTarget > mc_motorA_vBase) {
    //set acceleration to accerlerating
    mc_motorA_acceleration = ACCEL;
    //check is target velocity is lower than base velocity
  } else if (mc_motorA_vTarget < mc_motorA_vBase) {
    //set acceleration to decerlerating
    mc_motorA_acceleration = DECEL;
  } else {
    //else set acceleration to constant
    mc_motorA_acceleration = CONST;
  }
  //check if acceleration is not constant
  if (mc_motorA_acceleration != CONST) {
    //calculate the multiplier for the set acceleration
    mc_motorA_r = mc_motorA_acc / (mc_timerFrequenz * mc_timerFrequenz);
    //calculate initial delay
    mc_motorA_del_init = mc_timerFrequenz / sqrt((mc_motorA_vBase * mc_motorA_vBase) + 2 * mc_motorA_acc);
    //check if acceleration is accelerating
    if (mc_motorA_acceleration == ACCEL) {
      //calculate the first delay from inital delay
      mc_motorA_del_current = mc_motorA_del_init * ( 1 + (-mc_motorA_r) * mc_motorA_del_init * mc_motorA_del_init);
      //calculate steps to make during speed transition
      mc_motorA_steps = ((mc_motorA_vTarget * mc_motorA_vTarget) - (mc_motorA_vBase * mc_motorA_vBase)) / (2 * mc_motorA_acc);
      //check if acceleration is decelerating
    } else if (mc_motorA_acceleration == DECEL) {
      //calculate the first delay from inital delay
      mc_motorA_del_current = mc_motorA_del_init * ( 1 + (mc_motorA_r) * mc_motorA_del_init * mc_motorA_del_init);
      //calculate steps to make during speed transition
      mc_motorA_steps = ((mc_motorA_vBase * mc_motorA_vBase) - (mc_motorA_vTarget * mc_motorA_vTarget)) / (2 * mc_motorA_acc);
    }
    //reset step counter
    mc_motorA_step_counter = 0;
  }
  sei();
}

void motorB_speed(uint16_t target) {
  cli();
  mc_motorB_vBase = mc_motorB_vTarget;
  mc_motorB_vTarget = target;
  if (mc_motorB_vTarget > mc_motorB_vBase) {
    mc_motorB_acceleration = ACCEL;
  } else if (mc_motorB_vTarget < mc_motorB_vBase) {
    mc_motorB_acceleration = DECEL;
  } else {
    mc_motorB_acceleration = CONST;
  }
  if (mc_motorB_acceleration != CONST) {
    mc_motorB_r = mc_motorB_acc / (mc_timerFrequenz * mc_timerFrequenz);
    mc_motorB_del_init = mc_timerFrequenz / sqrt((mc_motorB_vBase * mc_motorB_vBase) + 2 * mc_motorB_acc);
    if (mc_motorB_acceleration == ACCEL) {
      mc_motorB_del_current = mc_motorB_del_init * ( 1 + (-mc_motorB_r) * mc_motorB_del_init * mc_motorB_del_init);
      mc_motorB_steps = ((mc_motorB_vTarget * mc_motorB_vTarget) - (mc_motorB_vBase * mc_motorB_vBase)) / (2 * mc_motorB_acc);
    } else if (mc_motorB_acceleration == DECEL) {
      mc_motorB_del_current = mc_motorB_del_init * ( 1 + (mc_motorB_r) * mc_motorB_del_init * mc_motorB_del_init);
      mc_motorB_steps = ((mc_motorB_vBase * mc_motorB_vBase) - (mc_motorB_vTarget * mc_motorB_vTarget)) / (2 * mc_motorB_acc);
    }
    mc_motorB_step_counter = 0;
  }
  sei();
}

void motorA_acceleration(float acc) {
  mc_motorA_acc = acc;
}

void motorB_acceleration(float acc) {
  mc_motorB_acc = acc;
}
