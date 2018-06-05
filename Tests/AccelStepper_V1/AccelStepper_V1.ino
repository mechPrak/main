#include <AccelStepper.h>

// Motor A
#define STEP_A 2 // PD2
#define DIR_A 3 // PD3
#define M0_A PD4
#define M1_A PD5
#define ENABLE_A PD6
AccelStepper motor_A(AccelStepper::DRIVER, STEP_A, DIR_A);

// Motor B
#define STEP_B 8 // PB0
#define DIR_B 9 // PB1
#define M0_B PB2
#define M1_B PB3
#define ENABLE_B PB4
AccelStepper motor_B(AccelStepper::DRIVER, STEP_B, DIR_B);

uint16_t s = 1000;

uint16_t premillis;
boolean state;

void setup() {
  DDRD |= (1 << ENABLE_A);
  DDRB |= (1 << ENABLE_B);

  pinMode(13, OUTPUT);

  motor_A.setMaxSpeed(s);
  motor_A.setAcceleration(7000.0);
  motor_A.moveTo(10000);

  Motor_enableA();
  Motor_microStep_A(4);


  motor_B.setMaxSpeed(1000.0);
  motor_B.setAcceleration(7000.0);
  motor_B.moveTo(-10000);

  //Motor_enableB();
  Motor_microStep_B(4);
}

void loop() {
  if (motor_A.distanceToGo() == 0)
    motor_A.moveTo(-motor_A.currentPosition());

  motor_A.run();

  if (motor_B.distanceToGo() == 0)
    motor_B.moveTo(-motor_B.currentPosition());

  motor_B.run();

  /*if (millis() - premillis >= 1000) {
    premillis = millis();
    state = !state;
    digitalWrite(13, state);

    if (state) {
      s += 10;
    } else {
      s -= 10;
    }
    motor_A.setMaxSpeed(s);
    motor_B.setMaxSpeed(s);
  }*/
}

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
