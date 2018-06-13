//mov und move to
//get pos und set pos
// compensate


HardwareTimer motor_timer(2);

//Hardware Pins
#define MR_STEP PB8
#define MR_DIR PB9
#define MR_ENABLE PB7



#define MC_MIN_DELAY  10
#define MC_SNEAK_DELAY  200
#define MC_INITIAL_DELAY 500

//TODO: variablentypen optimieren
volatile int mr_dir = 0;						//1 oder -1, bestimmt Richtung
unsigned int mr_c0;								//Initialer Delay
volatile unsigned long mr_n = 0;				//
volatile unsigned int mr_minDelay;				//Minimaler Delay -> maximale Geschwindigkeit
volatile unsigned long mr_stepCount = 0;		//
volatile unsigned long mr_rampUpStepCount = 0;	//
volatile float mr_delay = 0;					//
volatile unsigned long mr_totalSteps = 0;		//
volatile int mr_stepPosition = 0;				//
volatile bool mr_movementDone = false;			//
volatile int mr_counter = 0;					//Counter für Delay
volatile bool mr_sneak = false;
volatile int mr_sneakDelay = MC_SNEAK_DELAY;
void setup() {
  m_init();
  mr_enable();
}

void m_init() {
  pinMode(MR_STEP, OUTPUT);
  pinMode(MR_DIR, OUTPUT);
  pinMode(MR_ENABLE, OUTPUT);

  mr_c0 = MC_INITIAL_DELAY;
  mr_minDelay = MC_MIN_DELAY;

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
	  
	  
	  //Prüfe ob noch ein Step gefahren werden muss
    if (mr_stepCount < mr_totalSteps) {
      digitalWrite(MR_STEP, HIGH);							//STEP fahren
      digitalWrite(MR_STEP, LOW);
      mr_stepCount++;										//Relative Position updaten
      mr_stepPosition += mr_dir;							//Absolute Position updaten
    }
	//Motor rechts Bewegung abgeschlossen
    else {
      mr_movementDone = true;								//
    }
	
	//Beschleunigen
    if (mr_rampUpStepCount == 0) {
      mr_n++;
      mr_delay = mr_delay - (2 * mr_delay) / (4 * mr_n + 1);
	  
	  //Maximale Geschwindigkeit wurde erreicht
      if (mr_delay <= mr_minDelay) {
        mr_delay = mr_minDelay;
        mr_rampUpStepCount = mr_stepCount;
      }
	  
	  //Vorzeitiges Abbremsen (Falls mr_totalSteps nicht ausreicht um auf maximale Geschwindigkeit zu beschleunigen)
      if (mr_stepCount >= mr_totalSteps / 2) {
        mr_rampUpStepCount = mr_stepCount;
      }
    }
	
	//Bremsen
    else
		//sneak-Modus
		if(mr_delay <= MC_SNEAK_DELAY){
			mr_delay = MC_SNEAK_DELAY;
			if(!mr_sneak){
				//Motor stoppen
				mr_stepCount = mr_totalSteps;
			}
		}
		//weiteres Abbremsen
		else if (mr_stepCount >= mr_totalSteps - mr_rampUpStepCount ) {
			mr_n--;
			mr_delay = (mr_delay * (4 * mr_n + 1)) / (4 * mr_n + 1 - 2);
		}
	
	//Neuen Counter für Delay setzen
    mr_counter = 0;
  }
  
  //Counter für Delay inkrementieren
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
  mr_sneak = true;
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
  mr_setAcc(1000);
  mr_move(3000);
  delay(4000);
  setSneak(false);

  delay(10000);
}




//-----------------------
//		GETTER
//-----------------------



//-----------------------
//		SETTER
//-----------------------

void setSneak(bool nSneak){
	mr_sneak = nSneak;
}


