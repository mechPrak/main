//enabel
//set acc und set max speed
//mov und move to
//get pos und set pos


HardwareTimer motor_timer(2);

#define M_ACC 500
#define M_MAX_SPEED 10

#define MR_STEP PC13
#define MR_DIR PC14

//#define MR_ENABLE PC15

unsigned int mr_c0;
volatile unsigned int mr_maxSpeed;

volatile int mr_dir = 0;

volatile unsigned long n = 0;
volatile float d;
volatile unsigned long stepCount = 0;
volatile unsigned long rampUpStepCount = 0;
volatile unsigned long totalSteps = 0;
volatile int stepPosition = 0;

volatile bool movementDone = false;

volatile int counter = 0;

void setup() {
  m_init();
}

void m_init() {
  pinMode(MR_STEP, OUTPUT);
  pinMode(MR_DIR, OUTPUT);

  mr_c0 = M_ACC;
  mr_maxSpeed = M_MAX_SPEED;

  motor_timer.pause();
  motor_timer.setPeriod(10);
  motor_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  motor_timer.setCompare(TIMER_CH1, 1);
  motor_timer.attachCompare1Interrupt(motor);
  motor_timer.refresh();
  motor_timer.resume();

  c0 = M_ACC;
}


void motor() {
  if (counter >= d) {
    if ( stepCount < totalSteps ) {
      digitalWrite(MR_STEP, HIGH);
      digitalWrite(MR_STEP, LOW);
      stepCount++;
      stepPosition += dir;
    }
    else {
      movementDone = true;
    }

    if ( rampUpStepCount == 0 ) { // ramp up phase
      n++;
      d = d - (2 * d) / (4 * n + 1);
      if ( d <= maxSpeed ) { // reached max speed
        d = maxSpeed;
        rampUpStepCount = stepCount;
      }
      if ( stepCount >= totalSteps / 2 ) { // reached halfway point
        rampUpStepCount = stepCount;
      }
    }
    else if ( stepCount >= totalSteps - rampUpStepCount ) { // ramp down phase
      n--;
      d = (d * (4 * n + 1)) / (4 * n + 1 - 2);
    }

    counter = 0;
  }
  counter++;
}

void moveNSteps(long steps) {
  digitalWrite(MR_DIR, steps < 0 ? HIGH : LOW);
  mr_dir = steps > 0 ? 1 : -1;
  totalSteps = abs(steps);
  d = c0;
  stepCount = 0;
  n = 0;
  rampUpStepCount = 0;
  movementDone = false;
}

void moveToPosition(long p, bool wait = true) {
  moveNSteps(p - stepPosition);
  while ( wait && ! movementDone );
}

void loop() {
  maxSpeed = 10;
  moveToPosition(  3200 );
  moveToPosition( -3200 );
}
