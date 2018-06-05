/*
  Motortreiber A:
  2  -> STEP
  3  -> DIR
  4  -> M0
  5  -> M1
  6  -> ENABLE

  Motortreiber B:
  8  -> STEP
  9  -> DIR
  10 -> M0
  11 -> M1
  12 -> ENABLE

  Akku Spannung
  A5 -> AKSPA
*/

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

#define AKKSPA A5

volatile uint8_t setSpeed_A;
volatile uint8_t setSpeed_B;
volatile uint8_t stepDivider_A = 50;
volatile uint8_t stepCounter_A = 0;
volatile uint8_t stepDivider_B = 50;
volatile uint8_t stepCounter_B = 0;

void setup() {
  initServo();

  initMotor();
  Motor_enableA();
  Motor_enableB();
  Motor_microStep_A(16);
  Motor_microStep_B(16);

  OCR0A = 1;
  OCR2A = 1;

  sei();
}

void loop() {
//    Motor_enableA();
//    Motor_enableB();
//  
//    stepDivider_A = 100;
//    stepDivider_B = 100;
//    Motor_microStep_A(16);
//    Motor_microStep_B(16);
//    OCR0A = 2;
//    OCR2A = 2;
//    for (uint8_t j = 0; j < 90; j++) {
//      for (volatile uint32_t i = 0; i < 500; i++) {
//      }
//      stepDivider_A--;
//      stepDivider_B--;
//    }
//  
//    stepDivider_A = 100;
//    stepDivider_B = 100;
//    Motor_microStep_A(8);
//    Motor_microStep_B(8);
//    OCR0A = 1;
//    OCR2A = 1;
//    for (uint8_t j = 0; j < 90; j++) {
//      for (volatile uint32_t i = 0; i < 500; i++) {
//      }
//      stepDivider_A--;
//      stepDivider_B--;
//    }
//  
//    for (volatile uint32_t i = 0; i < 100000; i++) {
//    }
//    Motor_disableA();
//    Motor_disableB();
//    for (volatile uint32_t i = 0; i < 500000; i++) {
//    }


  //
  //
  //  stepDivider_A = 10;
  //  stepDivider_B = 10;
  //  OCR0A = 50;
  //  OCR2A = 50;
  //  Motor_enableA();
  //  Motor_enableB();
  //  for (uint8_t j = 0; j < 55; j++) {
  //    for (volatile uint32_t i = 0; i < 3000; i++) {
  //    }
  //    if (OCR0A > 1) {
  //      OCR0A--;
  //    }
  //    if (OCR2A > 1) {
  //      OCR2A--;
  //    }
  //  }
  //  stepDivider_A--;
  //  stepDivider_B--;
  //  for (volatile uint32_t i = 0; i < 10000; i++) {
  //  }
  //  stepDivider_A--;
  //  stepDivider_B--;
  //  for (volatile uint32_t i = 0; i < 10000; i++) {
  //  }
  //  stepDivider_A--;
  //  stepDivider_B--;
  //  for (volatile uint32_t i = 0; i < 100000; i++) {
  //  }
  //  Motor_disableA();
  //  Motor_disableB();
  //  stepDivider_A += 3;
  //  stepDivider_B += 3;
  //  for (volatile uint32_t i = 0; i < 200000; i++) {
  //  }
}
