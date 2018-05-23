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

volatile uint8_t s = 0;

uint32_t prevMillis = 0;

void setup() {
  initMotor();
  Motor_enableA();


  OCR0A = 10;
  //OCR2A = 50;

  s = 255;

  sei();
}

void loop() {
  //  if (millis() - prevMillis >= 100) {
  //    prevMillis = millis();
  //    s += 10;
  //  }
  Motor_microStep_A(1);
  for (volatile uint32_t i = 0; i < 200000; i++) {
  }
  Motor_microStep_A(2);
  for (volatile uint32_t i = 0; i < 200000; i++) {
  }
  Motor_microStep_A(4);
  for (volatile uint32_t i = 0; i < 200000; i++) {
  }
  Motor_microStep_A(8);
  for (volatile uint32_t i = 0; i < 200000; i++) {
  }
  Motor_microStep_A(16);
  for (volatile uint32_t i = 0; i < 200000; i++) {
  }
  Motor_microStep_A(32);
  for (volatile uint32_t i = 0; i < 200000; i++) {
  }
  //  s--;
}
