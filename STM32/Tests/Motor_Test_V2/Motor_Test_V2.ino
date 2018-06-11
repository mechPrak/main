//mov und move to
//get pos und set pos
// compensate


HardwareTimer motor_timer(2);

#define MR_STEP PC13
#define MR_DIR PC14
#define MR_ENABLE PC15

volatile int mr_dir = 0;
unsigned int mr_c0;
volatile unsigned long mr_n = 0;
volatile unsigned int mr_minDelay;
volatile unsigned long mr_stepCount = 0;
volatile unsigned long mr_rampUpStepCount = 0;
volatile float mr_delay = 0;
volatile unsigned long mr_totalSteps = 0;
volatile int mr_stepPosition = 0;
volatile bool mr_movementDone = false;
volatile int mr_counter = 0;

void setup() {
  m_init();
  mr_enable();
}

void m_init() {
  pinMode(MR_STEP, OUTPUT);
  pinMode(MR_DIR, OUTPUT);
  pinMode(MR_ENABLE, OUTPUT);

  mr_c0 = 500;
  mr_minDelay = 10;

  motor_timer.pause();
  motor_timer.setPeriod(10);
  motor_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  motor_timer.setCompare(TIMER_CH1, 1);
  motor_timer.attachCompare1Interrupt(motor);
  motor_timer.refresh();
  motor_timer.resume();
}


void motor() {
  if (mr_counter >= mr_delay) {
    if (mr_stepCount < mr_totalSteps) {
      digitalWrite(MR_STEP, HIGH);
      digitalWrite(MR_STEP, LOW);
      mr_stepCount++;
      mr_stepPosition += mr_dir;
    }
    else {
      mr_movementDone = true;
    }
    if (mr_rampUpStepCount == 0) {
      mr_n++;
      mr_delay = mr_delay - (2 * mr_delay) / (4 * mr_n + 1);
      if (mr_delay <= mr_minDelay) {
        mr_delay = mr_minDelay;
        mr_rampUpStepCount = mr_stepCount;
      }
      if (mr_stepCount >= mr_totalSteps / 2) {
        mr_rampUpStepCount = mr_stepCount;
      }
    }
    else if (mr_stepCount >= mr_totalSteps - mr_rampUpStepCount ) {
      mr_n--;
      mr_delay = (mr_delay * (4 * mr_n + 1)) / (4 * mr_n + 1 - 2);
    }
    mr_counter = 0;
  }
  mr_counter++;
}

void mr_move(long steps) {
  digitalWrite(MR_DIR, steps < 0 ? HIGH : LOW);
  mr_dir = steps > 0 ? 1 : -1;
  mr_totalSteps = abs(steps);
  mr_delay = mr_c0;
  mr_stepCount = 0;
  mr_n = 0;
  mr_rampUpStepCount = 0;
  mr_movementDone = false;
}

void mr_setAcc(int acc){
  mr_c0 = acc;
}

void mr_setMinDelay(int del){
  mr_minDelay = del;
}

void mr_enable(){
  digitalWrite(MR_ENABLE, LOW);
}

void mr_disable(){
  digitalWrite(MR_ENABLE, HIGH);
}


void loop() {
  mr_setMinDelay(10);
  mr_setAcc(100);
  mr_move(3200);
  delay(2000);
  mr_setMinDelay(20);
  mr_setAcc(2000);
  mr_move(3200);
  delay(2000);
}
