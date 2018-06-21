HardwareTimer motor_timer(2);

//Hardware Pins
#define PIN_MR_STEP PB10
#define PIN_MR_DIR PB1
#define PIN_MR_ENABLE PB11

#define PIN_ML_STEP PA7
#define PIN_ML_DIR PA6
#define PIN_ML_ENABLE PB0


//Geschwindigkeitswerte
#define MC_MIN_DELAY  100
#define MC_SNEAK_DELAY  1000
#define MC_INITIAL_DELAY 2000


volatile uint32_t mc_delayCounter[2] = {0,0};
volatile uint32_t mc_rampingDelayCounter[2] = {0,0};
volatile uint32_t mc_rampingTablePos[2] = {0,0};

volatile uint32_t mc_rampingTableSteps = 0;
volatile uint32_t mc_stepsTotal[2] = {0,0};
volatile uint32_t mc_stepsTotalHalf[2] = {0,0};
volatile uint32_t mc_stepsMade[2] = {0,0};

volatile uint32_t rampingTableLenght = 0;
volatile uint32_t mc_rampingTable[300][2];

volatile float mc_compensation = 0;
volatile int32_t mc_compensationInt = 0;
volatile uint32_t mc_sneakEnable[2] = {0,0};
volatile uint32_t mc_sneakFlagDisable[2] = {0,0};

volatile uint32_t debugVariable1 = 0;
volatile uint32_t debugVariable2 = 0;



//State-System für MotorController
enum  {
	MC_RAMP_UP,
	MC_COAST,
	MC_RAMP_DOWN,
	MC_SNEAK,
	MC_STOP
};
uint8_t mc_currentState[2];

void setup() {
	Serial.begin(115200);  
	delay(1000);
	m_init();
	
	pinMode(PB9,OUTPUT);
}

void loop() {
	debugVariable1 = 0;
	debugVariable2 = 0;
	mc_move(0,10000);	
	mc_sneakEnable[0] = false;
	mc_sneakFlagDisable[0] = false;
	mc_move(1,10000);	
	mc_sneakEnable[1] = false;
	mc_sneakFlagDisable[1] = false;
	
	for(volatile int i = 0; i < 100; i++){
		mc_compensationInt = 10;
		delay(100);
		mc_compensationInt = -10;
		delay(100);
	}
	
	
	delay(5000);
	
	/*
	for(volatile int i = 0; i < 100000; i++){
		Serial.print(debugVariable1);
		Serial.print(", ");
		Serial.println(debugVariable2);
	}	
	*/
}


void m_init() {
	//pinModes einstellen
	pinMode(PIN_MR_STEP, OUTPUT);
	pinMode(PIN_MR_DIR, OUTPUT);
	pinMode(PIN_MR_ENABLE, OUTPUT);
	
	pinMode(PIN_ML_STEP, OUTPUT);
	pinMode(PIN_ML_DIR, OUTPUT);
	pinMode(PIN_ML_ENABLE, OUTPUT);
	
	digitalWrite(PIN_MR_DIR, HIGH);
	
	calculateRampingTable();
	
	mc_currentState[0] = MC_STOP;
	mc_currentState[1] = MC_STOP;

	//Interrupt initialisieren
	motor_timer.pause();
	motor_timer.setPeriod(5);
	motor_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
	motor_timer.setCompare(TIMER_CH1, 1);
	motor_timer.attachCompare1Interrupt(motor_ISR);
	motor_timer.refresh();
	motor_timer.resume();
}

void calculateRampingTable(){
	float delay = MC_INITIAL_DELAY;
	uint32_t currentDelayInt = 0;
	
	rampingTableLenght --;
	while((uint16_t)delay > MC_MIN_DELAY){
		if((uint16_t)delay == currentDelayInt){
			mc_rampingTable[rampingTableLenght][1] ++;
		} 
		else {
			rampingTableLenght ++;
			mc_rampingTable[rampingTableLenght][0] = (uint16_t)delay;
			mc_rampingTable[rampingTableLenght][1] ++;
			currentDelayInt = (uint16_t)delay;			
		}
		
		mc_rampingTableSteps++;	
		delay = delay - (2 * delay) / (4 * mc_rampingTableSteps + 1);				
	}
	rampingTableLenght++;
	
	
	//DEBUG
	/*
	Serial.print("Total steps: ");
	Serial.println(mc_rampingTableSteps);
	Serial.print("Delay entries: ");
	Serial.println(rampingTableLenght);
	for(uint32_t i = 0; i < rampingTableLenght; i++){
		Serial.print(mc_rampingTable[i][0]);
		Serial.print(", ");
		Serial.print(mc_rampingTable[i][1]);
		Serial.println();
	}
	*/
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
				if(mc_delayCounter[i] > MC_MIN_DELAY + mc_compensationInt){
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
				if(mc_delayCounter[i] > mc_rampingTable[mc_rampingTablePos[i]][0]){
					mc_motorStep(i);				
					mc_stepsMade[i]++;										
					if(mc_rampingDelayCounter[i] == 0){
						mc_rampingTablePos[i] --;
						mc_rampingDelayCounter[i] = mc_rampingTable[mc_rampingTablePos[i]][1];
						if(mc_sneakEnable[i] && mc_rampingTable[mc_rampingTablePos[i]][0] >= MC_SNEAK_DELAY){
							mc_currentState[i] = MC_SNEAK;
						}
						else if(mc_stepsMade[i] >= mc_stepsTotal[i]){
							mc_currentState[i] = MC_STOP;
						}
					}			
					mc_rampingDelayCounter[i] --;					
					mc_delayCounter[i] = 0;
				}
				mc_delayCounter[i] ++;
				break;
				
			case MC_SNEAK:
				if(mc_delayCounter[i] > MC_SNEAK_DELAY){
					if(mc_sneakFlagDisable[i]){
						mc_sneakFlagDisable[i] = false;
						mc_currentState[i] = MC_STOP;
						break;
					}
					mc_motorStep(i);				
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
		break;
		case 1:
			digitalWrite(PIN_ML_STEP, HIGH);
			digitalWrite(PIN_ML_STEP, LOW);
		break;
	}  
}


