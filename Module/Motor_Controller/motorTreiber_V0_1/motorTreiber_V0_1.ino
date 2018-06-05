#include <MultiStepper.h>
#include <AccelStepper.h>

#define PIN_A_M0 A0
#define PIN_A_M1 A1
#define PIN_B_M0 A2
#define PIN_B_M1 A3

#define PIN_A_STEP 2
#define PIN_A_DIR 4
#define PIN_B_STEP 7
#define PIN_B_DIR 8

#define PIN_V_meas_1 0
#define PIN_V_meas_2 0

void setup(){
  pinMode(PIN_A_M0, OUTPUT);  
  pinMode(PIN_B_M1, OUTPUT);
  pinMode(PIN_A_M0, OUTPUT);
  pinMode(PIN_B_M1, OUTPUT);
  
  pinMode(PIN_A_STEP, OUTPUT);  
  pinMode(PIN_A_DIR, OUTPUT);
  pinMode(PIN_B_STEP, OUTPUT);
  pinMode(PIN_B_DIR, OUTPUT);
  
  pinMode(PIN_V_meas_1, INPUT;
  pinMode(PIN_V_meas_2, INPUT);
}


void loop(){
  
  
}
