enum {ACCEL, DECEL, CONST, CW, CCW};

float mc_timerFrequenz;

// Motor A
#define DDR_A DDRD
#define PORT_A PORTD
#define STEP_A PD2
#define DIR_A PD3
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
#define STEP_B PB0
#define DIR_B PB1
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

void setup() {
  motor_init();

  motorA_enable();
  motorA_microStep(16);

  motorB_enable();
  motorB_microStep(16);
}

void loop() {
}

void motor_init() {
  //set pins as output
  DDR_A |= ((1 << STEP_A) | (1 << DIR_A) | (1 << ENABLE_A));
  DDR_B |= ((1 << STEP_B) | (1 << DIR_B) | (1 << ENABLE_B));
}
void motorA_microStep(uint8_t stepSize) {
  switch (stepSize) {
    case 1:
      DDR_A |= (1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A &= ~(1 << M0_A);
      PORT_A &= ~(1 << M1_A);
      break;
    case 2:
      DDR_A |= (1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A |= (1 << M0_A);
      PORT_A &= ~(1 << M1_A);
      break;
    case 4:
      DDR_A &= ~(1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A &= ~(1 << M0_A);
      PORT_A &= ~(1 << M1_A);
      break;
    case 8:
      DDR_A |= (1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A &= ~(1 << M0_A);
      PORT_A |= (1 << M1_A);
      break;
    case 16:
      DDR_A |= (1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A |= (1 << M0_A);
      PORT_A |= (1 << M1_A);
      break;
    case 32:
      DDR_A &= ~(1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A &= ~(1 << M0_A);
      PORT_A |= (1 << M1_A);
      break;
  }
}

void motorB_microStep(uint8_t stepSize) {
  switch (stepSize) {
    case 1:
      DDR_B |= (1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B &= ~(1 << M0_B);
      PORT_B &= ~(1 << M1_B);
      break;
    case 2:
      DDR_B |= (1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B |= (1 << M0_B);
      PORT_B &= ~(1 << M1_B);
      break;
    case 4:
      DDR_B &= ~(1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B &= ~(1 << M0_B);
      PORT_B &= ~(1 << M1_B);
      break;
    case 8:
      DDR_B |= (1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B &= ~(1 << M0_B);
      PORT_B |= (1 << M1_B);
      break;
    case 16:
      DDR_B |= (1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORTB |= (1 << M0_B);
      PORTB |= (1 << M1_B);
      break;
    case 32:
      DDR_B &= ~(1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B &= ~(1 << M0_B);
      PORT_B |= (1 << M1_B);
      break;
  }
}

void motorA_enable() {
  PORT_A |= (1 << ENABLE_A);
}

void motorB_enable() {
  PORT_B |= (1 << ENABLE_B);
}

void motorA_disable() {
  PORT_A &= ~(1 << ENABLE_A);
}

void motorB_disable() {
  PORT_B &= ~(1 << ENABLE_B);
}

