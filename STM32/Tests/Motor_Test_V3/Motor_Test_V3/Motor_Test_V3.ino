HardwareTimer motor_timer(2);

//Hardware Pins
#define PIN_MR_STEP PB10
#define PIN_MR_DIR PB1
#define PIN_MR_ENABLE PB11

#define PIN_ML_STEP PA7
#define PIN_ML_DIR PA6
#define PIN_ML_ENABLE PB0


//Geschwindigkeitswerte
#define MC_MIN_DELAY  10
#define MC_SNEAK_DELAY  20
#define MC_INITIAL_DELAY 1000

//Rampingtabelle
#define MC_RAMPING_TABLE_ENTRIES 119

volatile uint32_t mc_delayCounter[2] = {0,0};
volatile uint32_t mc_rampingDelayCounter[2] = {0,0};
volatile uint32_t mc_rampingTablePos[2] = {0,0};

volatile uint32_t mc_rampingTableSteps = 1;
volatile uint32_t mc_maxSpeedSteps[2] = {0,0};
volatile uint32_t mc_stepsTotal[2] = {0,0};
volatile uint32_t mc_stepsTotalHalf[2] = {0,0};
volatile uint32_t mc_stepsMade[2] = {0,0};


volatile float mc_compensation[2] = {0,0};
volatile uint32_t mc_sneakEnable[2] = {0,0};
volatile uint32_t mc_flagSneakDisable[2] = {0,0};

volatile uint32_t debugVariable1 = 0;
volatile uint32_t debugVariable2 = 0;

//volatile uint32_t *mc_rampingTable;
volatile uint32_t rampingTableLenght = 0;
volatile uint32_t mc_rampingTable[200][2];

//State-System für MotorController
enum  {
	MC_RAMP_UP,
	MC_COAST,
	MC_RAMP_DOWN,
	MC_SNEAK,
	MC_STOP
};
uint8_t mc_currentState[2];

/*
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
*/

void setup() {
	Serial.begin(115200);  
	delay(1000);
	m_init();
}

void loop() {
	debugVariable1 = 0;
	debugVariable2 = 0;
	mc_move(0,100000);
	mc_move(1,100000);
	
	for(volatile int i = 0; i < 100000; i++){
		Serial.print(debugVariable1);
		Serial.print(", ");
		Serial.println(debugVariable2);
	}
	
}


void m_init() {
	//pinModes einstellen
	pinMode(PIN_MR_STEP, OUTPUT);
	pinMode(PIN_MR_DIR, OUTPUT);
	pinMode(PIN_MR_ENABLE, OUTPUT);
	
	pinMode(PIN_ML_STEP, OUTPUT);
	pinMode(PIN_ML_DIR, OUTPUT);
	pinMode(PIN_ML_ENABLE, OUTPUT);
	
	calculateRampingTable();
	
	mc_currentState[0] = MC_STOP;
	mc_currentState[1] = MC_STOP;

	//Interrupt initialisieren
	motor_timer.pause();
	motor_timer.setPeriod(10);
	motor_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
	motor_timer.setCompare(TIMER_CH1, 1);
	motor_timer.attachCompare1Interrupt(motor_ISR);
	motor_timer.refresh();
	motor_timer.resume();
}

void calculateRampingTable(){
	float delay = MC_INITIAL_DELAY;
	uint32_t currentDelayInt = 0;
	
	while(delay > MC_MIN_DELAY){
		if((uint16_t)delay != currentDelayInt){
			mc_rampingTable[rampingTableLenght][0] = (uint16_t)delay;
			mc_rampingTable[rampingTableLenght][1] ++;
			rampingTableLenght ++;
			currentDelayInt = delay;
		} else {
			mc_rampingTable[rampingTableLenght][1] ++;
		}
		
		delay = delay - (2 * delay) / (4 * mc_rampingTableSteps + 1);
		mc_rampingTableSteps++;		
	}
	
	//mc_rampingTableSteps -= mc_rampingTable[rampingTableLenght][1] + 2;
}

void motor_ISR(){
	// Motoren durchzählen
	for(uint8_t i = 0; i <= 1; i++){
		switch(mc_currentState[i]){
			case MC_STOP: break;
			
			case MC_RAMP_UP:
				// Prüfen, ob ein Step gefahren werden muss
				if(mc_delayCounter[i] > mc_rampingTable[mc_rampingTablePos[i]][0]){
					mc_motorStep(i);					
					mc_stepsMade[i]++;				
					mc_rampingDelayCounter[i] ++;				
					// Prüfen, ob der nächste Delay ausgewählt werden muss
					if(mc_rampingDelayCounter[i] >= mc_rampingTable[mc_rampingTablePos[i]][1]){
						mc_rampingTablePos[i] ++;
						mc_rampingDelayCounter[i] = 0;
						if(mc_stepsMade[i] == mc_rampingTableSteps){
							mc_currentState[i] = MC_COAST;
						}
					}				
					if(mc_stepsMade[i] == mc_stepsTotalHalf[i]){
						mc_currentState[i] = MC_RAMP_DOWN;
						mc_rampingDelayCounter[i] ++;
					}
					mc_delayCounter[i] = 0;
				}
				mc_delayCounter[i] ++;
				break;
				
			case MC_COAST:
				if(mc_delayCounter[i] > MC_MIN_DELAY){
					mc_motorStep(i);				
					mc_stepsMade[i]++;
					mc_delayCounter[i] = 0;
					if(mc_stepsMade[i] >= (mc_stepsTotal[i] - mc_rampingTableSteps)){
						mc_currentState[i] = MC_RAMP_DOWN;
					}
				}
				mc_delayCounter[i] ++;
				break;
				
			case MC_RAMP_DOWN:
				// Prüfen, ob ein Step gefahren werden muss
				if(mc_delayCounter[i] > mc_rampingTable[mc_rampingTablePos[i]][0]){
					debugVariable2 = mc_rampingTable[mc_rampingTablePos[i]][0];
					mc_motorStep(i);				
					mc_stepsMade[i]++;													
					// Prüfen, ob der nächste Delay ausgewählt werden muss
					if(mc_rampingDelayCounter[i] == 0){						
						mc_rampingTablePos[i] --;
						mc_rampingDelayCounter[i] = mc_rampingTable[mc_rampingTablePos[i]][1];
						if(mc_stepsMade[i] >= mc_stepsTotal[i]){
							mc_currentState[i] = MC_STOP;
						}
					}			
					mc_rampingDelayCounter[i] --;					
					mc_delayCounter[i] = 0;
				}
				mc_delayCounter[i] ++;
				break;
		}
	}
}

void mc_move(uint8_t motor, uint32_t steps){
	mc_delayCounter[motor] = 0;
	mc_rampingTablePos[motor] = 0;
	mc_rampingDelayCounter[motor] = 0;
	mc_stepsMade[motor] = 0;
	mc_stepsTotal[motor] = steps;	
	mc_stepsTotalHalf[motor] = steps / 2;	
	mc_currentState[motor] = MC_RAMP_UP;
}


void mc_motorStep(uint8_t motor){
	switch(motor){
		case 0:
			digitalWrite(PIN_MR_STEP, HIGH);
			digitalWrite(PIN_MR_STEP, LOW);
			debugVariable1++;
		break;
		case 1:
			digitalWrite(PIN_ML_STEP, HIGH);
			digitalWrite(PIN_ML_STEP, LOW);
		break;
	}  
}


