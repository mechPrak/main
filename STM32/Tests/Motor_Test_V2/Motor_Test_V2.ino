//mov und move to
//get pos und set pos
// compensate



HardwareTimer motor_timer(2);

//Hardware Pins
#define MR_STEP PB10
#define MR_DIR PB1
#define MR_ENABLE PB11

#define ML_STEP PA7
#define ML_DIR PA6
#define ML_ENABLE PB0


//Geschwindigkeitswerte
#define MC_MIN_DELAY  10
#define MC_SNEAK_DELAY  20
#define MC_INITIAL_DELAY 500

//TODO: variablentypen optimieren
volatile int mr_dir = 0;						        //1 oder -1, bestimmt Richtung
unsigned int mr_c0;								          //Initialer Delay
volatile unsigned long mr_n = 0;				    //
volatile unsigned int mr_minDelay;				  //Minimaler Delay -> maximale Geschwindigkeit
volatile unsigned long mr_stepCount = 0;		//
volatile unsigned long mr_rampUpStepCount = 0;	//
volatile float mr_delay = 0;					      //
volatile unsigned long mr_totalSteps = 0;		//
volatile int mr_stepPosition = 0;				    //
volatile bool mr_movementDone = false;			//
volatile int mr_counter = 0;					      //Counter für Delay
volatile bool mrSneakEnabled = false;
volatile bool mrStopSneak = false;
volatile int mr_sneakDelay = MC_SNEAK_DELAY;


volatile int ml_dir = 0;                    //1 oder -1, bestimmt Richtung
unsigned int ml_c0;                         //Initialer Delay
volatile unsigned long ml_n = 0;            //
volatile unsigned int ml_minDelay;          //Minimaler Delay -> maximale Geschwindigkeit
volatile unsigned long ml_stepCount = 0;    //
volatile unsigned long ml_rampUpStepCount = 0;  //
volatile float ml_delay = 0;                //
volatile unsigned long ml_totalSteps = 0;   //
volatile int ml_stepPosition = 0;           //
volatile bool ml_movementDone = false;      //
volatile int ml_counter = 0;                //Counter für Delay
volatile bool mlSneakEnabled = false;
volatile bool mlStopSneak = false;
volatile int ml_sneakDelay = MC_SNEAK_DELAY;


//DEBUG, REMOVE LATER
volatile int state = 0;

void setup() {
  m_init();
  mr_enable();
  ml_enable();

  Serial.begin(115200);

}

void m_init() {
  //pinModes einstellen
  pinMode(MR_STEP, OUTPUT);
  pinMode(MR_DIR, OUTPUT);
  pinMode(MR_ENABLE, OUTPUT);

  pinMode(ML_STEP, OUTPUT);
  pinMode(ML_DIR, OUTPUT);
  pinMode(ML_ENABLE, OUTPUT);

  //Motortreiberwerte setzen
  mr_c0 = MC_INITIAL_DELAY;
  mr_minDelay = MC_MIN_DELAY;
  ml_c0 = MC_INITIAL_DELAY;
  ml_minDelay = MC_MIN_DELAY;

  //Interrupt initialisieren
  motor_timer.pause();
  motor_timer.setPeriod(10);
  motor_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  motor_timer.setCompare(TIMER_CH1, 1);
  motor_timer.attachCompare1Interrupt(motor);
  motor_timer.refresh();
  motor_timer.resume();
}

void motor(){
  motorR();
  motorL();
}

void motorR() {
  state = ml_delay;
  
  //Prüfe, ob es an der Zeit ist, einen Step zu fahren
  if (mr_counter >= mr_delay) {
	  //Prüfe ob noch ein Step gefahren werden muss
    if (mr_stepCount < mr_totalSteps || (mrSneakEnabled && !mrStopSneak)) {
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
      //Langsame Geschwindigkeit halten (Schleichen)
    else if(mr_delay >= mr_sneakDelay && mrSneakEnabled == true){
        
       //Schleich-Delay setzen
       mr_delay = mr_sneakDelay;
      
      //Schleichen beenden
      if(mrStopSneak){
        //Abbruchbedingung setzen
        mr_stepCount = mr_totalSteps;
      }
    }
			//Abbremsen
		else if (mr_stepCount >= mr_totalSteps - mr_rampUpStepCount) {   
			  mr_n--;
			  mr_delay = (mr_delay * (4 * mr_n + 1)) / (4 * mr_n + 1 - 2);
		}
	
	//Neuen Counter für Delay setzen
    mr_counter = 0;
  }
  
  //Counter für Delay inkrementieren
  mr_counter++;
}

void motorL() {
  
  
  //Prüfe, ob es an der Zeit ist, einen Step zu fahren
  if (ml_counter >= ml_delay) {
    //Prüfe ob noch ein Step gefahren werden muss
    if (ml_stepCount < ml_totalSteps || (mlSneakEnabled && !mlStopSneak)) {
      digitalWrite(ML_STEP, HIGH);              //STEP fahren
      digitalWrite(ML_STEP, LOW);
      ml_stepCount++;                   //Relative Position updaten
      ml_stepPosition += ml_dir;              //Absolute Position updaten
    }
  //Motor rechts Bewegung abgeschlossen
    else {
      ml_movementDone = true;               //
    }
  
  //Beschleunigen
    if (ml_rampUpStepCount == 0) {
      ml_n++;
      ml_delay = ml_delay - (2 * ml_delay) / (4 * ml_n + 1);
    
    //Maximale Geschwindigkeit wurde erreicht
      if (ml_delay <= ml_minDelay) {
        ml_delay = ml_minDelay;
        ml_rampUpStepCount = ml_stepCount;
      }
    
    //Vorzeitiges Abbremsen (Falls ml_totalSteps nicht ausreicht um auf maximale Geschwindigkeit zu beschleunigen)
      if (ml_stepCount >= ml_totalSteps / 2) {
        ml_rampUpStepCount = ml_stepCount;
      }
    }

     //Langsame Geschwindigkeit halten (Schleichen)
    else if(ml_delay >= ml_sneakDelay && mlSneakEnabled == true){

        state = ml_delay;
        
       //Schleich-Delay setzen
       ml_delay = ml_sneakDelay;
      
      //Schleichen beenden
      if(mlStopSneak){
        //Abbruchbedingung setzen
        ml_stepCount = ml_totalSteps;
      }
    }
    
    //weiteres Abbremsen
    else if (ml_stepCount >= ml_totalSteps - ml_rampUpStepCount ) {
      ml_n--;
      ml_delay = (ml_delay * (4 * ml_n + 1)) / (4 * ml_n + 1 - 2);
    }
  
  //Neuen Counter für Delay setzen
    ml_counter = 0;
  }
  
  //Counter für Delay inkrementieren
  ml_counter++;
}


void mr_move(long steps) {
  //Motor ist um 180° gedreht eingebaut => Drehrichtung umkehren
  steps = -steps;
  
  digitalWrite(MR_DIR, steps < 0 ? HIGH : LOW);
  mr_dir = steps > 0 ? 1 : -1;
  mr_totalSteps = abs(steps);
  mr_delay = mr_c0;
  mr_stepCount = 0;
  mr_n = 0;
  mr_rampUpStepCount = 0;
  mr_movementDone = false;
}

void ml_move(long steps) {
  digitalWrite(ML_DIR, steps < 0 ? HIGH : LOW);
  ml_dir = steps > 0 ? 1 : -1;
  ml_totalSteps = abs(steps);
  ml_delay = mr_c0;
  ml_stepCount = 0;
  ml_n = 0;
  ml_rampUpStepCount = 0;
  ml_movementDone = false;
}

void mr_setAcc(int acc){
  mr_c0 = acc;
}

void ml_setAcc(int acc){
  ml_c0 = acc;
}

void mr_setMinDelay(int del){
  mr_minDelay = del;
}

void ml_setMinDelay(int del){
  ml_minDelay = del;
}

void mr_enable(){
  digitalWrite(MR_ENABLE, LOW);
}

void ml_enable(){
  digitalWrite(ML_ENABLE, LOW);
}

void mr_disable(){
  digitalWrite(MR_ENABLE, HIGH);
}


void ml_disable(){
  digitalWrite(ML_ENABLE, HIGH);
}


void loop() {
  
  mr_setMinDelay(10);
  mr_setAcc(1000);
  mr_move(10000);
  mr_setSneak(true);
  mr_setStopSneak(false);
  
  ml_setMinDelay(10);
  ml_setAcc(1000);
  ml_move(10000);
  ml_setSneak(true);
  ml_setStopSneak(false);
  
  for(int i = 0; i < 10000; i++){
    Serial.println(state);
  }
 
  //Schleichen deaktivieren
  mr_setStopSneak(true);
  ml_setStopSneak(true);
  delay(3000);
}




//-----------------------
//		GETTER
//-----------------------



//-----------------------
//		SETTER
//-----------------------

void mr_setSneak(bool sneakEnabled){
  mrSneakEnabled = sneakEnabled;
}

void ml_setSneak(bool sneakEnabled){
  mlSneakEnabled = sneakEnabled;
}

void mr_setStopSneak(bool stopSneak){
  mrStopSneak = stopSneak;
}

void ml_setStopSneak(bool stopSneak){
  mlStopSneak = stopSneak;
}
