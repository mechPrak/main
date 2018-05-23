#include <AccelStepper.h>

#define STEP_0 2
#define DIR_0 4
#define MS_0_0 A0
#define MS_0_1 A1

#define STEP_1 7
#define DIR_1 8
#define MS_1_0 A2
#define MS_1_1 A3

AccelStepper stepper0(AccelStepper::DRIVER, STEP_0, DIR_0);
AccelStepper stepper1(AccelStepper::DRIVER, STEP_1, DIR_1);

void setup() {
  stepper0.setMaxSpeed(20000.0);
  stepper0.setAcceleration(1000.0);
  stepper0.moveTo(-1000);

  stepper1.setMaxSpeed(20000.0);
  stepper1.setAcceleration(1000.0);
  stepper1.moveTo(1000);

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  digitalWrite(A0,HIGH);
  digitalWrite(A1,LOW);
  digitalWrite(A2,HIGH);
  digitalWrite(A3,LOW);
}

void loop() {
  if (stepper0.distanceToGo() == 0)
    stepper0.moveTo(-stepper0.currentPosition());

  if (stepper1.distanceToGo() == 0)
    stepper1.moveTo(-stepper1.currentPosition());

  stepper0.run();
  stepper1.run();
}
