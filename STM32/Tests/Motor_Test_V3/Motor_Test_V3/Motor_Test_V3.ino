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

//Rampingtabelle
#define MC_RAMPING_TABLE_ENTRIES 119

volatile uint16_t mc_delayCounter[2] = {0,0};
volatile uint16_t mc_rampingCounter[2] = {0,0};
volatile uint8_t mc_delayPos[2] = {0,0};

const uint16_t mc_rampingTable[][2] = {
  {466, 177},
  {394, 275},
  {348, 267},
  {315, 206},
  {290, 271},
  {270, 265},
  {253, 232},
  {239, 76},
  {228, 296},
  {209, 270},
  {201, 185},
  {187, 198},
  {182, 183},
  {176, 181},
  {167, 212},
  {159, 175},
  {155, 142},
  {152, 41},
  {149, 201},
  {143, 166},
  {141, 114},
  {136, 31},
  {131, 37},
  {127, 153},
  {125, 62},
  {124, 225},
  {120, 192},
  {117, 56},
  {116, 180},
  {112, 166},
  {109, 246},
  {106, 128},
  {103, 152},
  {101, 210},
  {99, 185},
  {97, 158},
  {96, 128},
  {95, 156},
  {93, 134},
  {92, 124},
  {90, 180},
  {89, 153},
  {88, 145},
  {87, 126},
  {86, 147},
  {85, 231},
  {84, 70},
  {83, 18},
  {82, 284},
  {81, 170},
  {80, 165},
  {79, 276},
  {78, 193},
  {76, 308},
  {75, 164},
  {74, 266},
  {73, 110},
  {72, 270},
  {71, 236},
  {70, 238},
  {69, 159},
  {68, 200},
  {67, 308},
  {66, 249},
  {65, 147},
  {64, 348},
  {63, 301},
  {62, 303},
  {61, 376},
  {60, 255},
  {59, 186},
  {58, 268},
  {57, 289},
  {56, 408},
  {55, 474},
  {54, 283},
  {53, 444},
  {52, 442},
  {51, 433},
  {50, 515},
  {49, 467},
  {48, 435},
  {47, 569},
  {46, 643},
  {45, 550},
  {44, 741},
  {43, 678},
  {42, 740},
  {41, 745},
  {40, 752},
  {39, 873},
  {38, 934},
  {37, 977},
  {36, 999},
  {35, 1068},
  {34, 1193},
  {33, 1325},
  {32, 1471},
  {31, 1449},
  {30, 1683},
  {29, 1587},
  {28, 1867},
  {27, 2107},
  {26, 2296},
  {25, 2527},
  {24, 2877},
  {23, 3233},
  {22, 3545},
  {21, 3723},
  {20, 4551},
  {19, 5238},
  {18, 6124},
  {17, 6962},
  {16, 7584},
  {15, 9684},
  {14, 11798},
  {13, 14090},
  {12, 17469},
  {11, 22438},
  {10, 1}
};

void setup() {
  Serial.begin(9600);
  
  m_init();
}

void loop() {


}


void m_init() {
  //pinModes einstellen
  pinMode(MR_STEP, OUTPUT);
  pinMode(MR_DIR, OUTPUT);
  pinMode(MR_ENABLE, OUTPUT);

  pinMode(ML_STEP, OUTPUT);
  pinMode(ML_DIR, OUTPUT);
  pinMode(ML_ENABLE, OUTPUT);

  //Interrupt initialisieren
  motor_timer.pause();
  motor_timer.setPeriod(10);
  motor_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  motor_timer.setCompare(TIMER_CH1, 1);
  motor_timer.attachCompare1Interrupt(motor_ISR);
  motor_timer.refresh();
  motor_timer.resume();
}

void motor_ISR(){
  //Motoren durchzählen
  for(int i = 0; i <= 1; i++){
    //Prüfen, ob ein Step gefahren werden muss
    if(mc_delayCounter[i] < mc_rampingTable[mc_delayPos[i]][0]){
      //Step fahren
      mc_motorStep(i);
      //Ramping-Counter hochzählen
      mc_rampingCounter[i]++;

      //Prüfen, ob der nächste Delay ausgewählt werden muss
      if(mc_rampingCounter[i] >= mc_rampingTable[mc_delayPos[i]][1]){
        //Ramping-Counter zurücksetzen
        mc_rampingCounter[i] = 0;
        //Zum nächsten Delay wechseln
        mc_delayPos[i]++;
      }

      //Delay-Counter zurücksetzen
      mc_delayCounter[i] = 0;
    }
    else{
      mc_delayCounter[i]++;
    }
  }
}


void mc_motorStep(uint8_t motor){
  if(motor == 0){
    digitalWrite(MR_STEP, HIGH);
    digitalWrite(MR_STEP, LOW);
  }
  if(motor = 1){
    digitalWrite(ML_STEP, HIGH);
    digitalWrite(ML_STEP, LOW);
  }
}


