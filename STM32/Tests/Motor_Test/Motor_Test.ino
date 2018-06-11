HardwareTimer timer(2);

//enum {ACCEL, DECEL, CONST, CW, CCW};
//float m_frq = 100000;

#define MR_STEP PC13
//#define MR_DIR PC14
//#define MR_ENABLE PC15

/*
  volatile long mr_vBase;
  volatile long mr_vTarget;
  volatile long mr_acc;
  volatile int mr_state;
  volatile long mr_r;
  volatile long mr_steps;
  volatile long mr_del_init;
  volatile long mr_del_current;
  volatile long mr_step_counter;
  volatile long mr_del_counter;
*/

int c0 = 1600;
float c = 0;
int n = 1;
int counter;

void setup() {
  m_init();
}

void m_init() {
  pinMode(PB6, OUTPUT);

  pinMode(MR_STEP, OUTPUT);


  //pinMode(MR_DIR, OUTPUT);
  //pinMode(MR_ENABLE, OUTPUT);

  //digitalWrite(MR_ENABLE, HIGH);

  //mr_acc = 1000;

  timer.pause();
  timer.setPeriod(10);
  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  timer.setCompare(TIMER_CH1, 1);
  timer.attachCompare1Interrupt(motor);
  timer.refresh();
  timer.resume();
}

void loop() {
  /*mr_speed(50000);
    delay(10000);
    mr_speed(0);
    delay(10000);*/
}

void motor() {
  digitalWrite(PB6, HIGH);

  if (counter > c) {
    counter = 0;
    //c = c * (sqrt(n + 1) - sqrt(n));
    c = c - (2 * c / (4 * n + 1));
    n++;
    digitalWrite(PC13, HIGH);
    digitalWrite(PC13, LOW);
  }
  counter++;

  digitalWrite(PB6, LOW);


  /*if (mr_del_counter > mr_del_current) {
    mr_del_counter = 0;

    if (mr_vTarget == 0 && mr_state == CONST) {
    } else {
      digitalWrite(PC13, HIGH);
      digitalWrite(PC13, LOW);
    }

    if (mr_step_counter < mr_steps) {

      if (mr_state == ACCEL) {
        mr_del_current = mr_del_current * ( 1 + (-mr_r) * mr_del_current * mr_del_current);
        //mr_del_current = mr_del_current - (2 * mr_d

      } else if (mr_state == DECEL) {
        mr_del_current = mr_del_current * ( 1 + mr_r * mr_del_current * mr_del_current);
      }

      mr_step_counter++;

    } else {
      mr_state = CONST;
    }
    }
    mr_del_counter ++;
  */
}

/*
  void mr_speed(long target) {
  timer.pause();

  mr_vBase = mr_vTarget;
  mr_vTarget = target;

  if (mr_vTarget > mr_vBase) {
    mr_state = ACCEL;
  } else if (mr_vTarget < mr_vBase) {
    mr_state = DECEL;
  } else {
    mr_state = CONST;
  }

  if (mr_state != CONST) {

    mr_r = mr_acc / (m_frq * m_frq);
    mr_del_init = m_frq / sqrt((mr_vBase * mr_vBase) + 2 * mr_acc);

    if (mr_state == ACCEL) {
      mr_del_current = mr_del_init * ( 1 + (-mr_r) * mr_del_init * mr_del_init);
      mr_steps = ((mr_vTarget * mr_vTarget) - (mr_vBase * mr_vBase)) / (2 * mr_acc);

    } else if (mr_state == DECEL) {
      mr_del_current = mr_del_init * ( 1 + mr_r * mr_del_init * mr_del_init);
      mr_steps = ((mr_vBase * mr_vBase) - (mr_vTarget * mr_vTarget)) / (2 * mr_acc);
    }

    mr_step_counter = 0;
  }
  timer.resume();
  }
*/
