HardwareTimer motor_timer(2);

//Hardware Pins
#define PIN_MR_STEP PB10
#define PIN_MR_DIR PB1
#define PIN_MR_ENABLE PB11

#define PIN_ML_STEP PA7
#define PIN_ML_DIR PA6
#define PIN_ML_ENABLE PB0

//Geschwindigkeitswerte
#define MC_MIN_DELAY  50
#define MC_SNEAK_DELAY  200
#define MC_INITIAL_DELAY 500


volatile uint32_t mc_delayCounter[2] = {0,0};
volatile uint32_t mc_rampingDelayCounter[2] = {0,0};
volatile uint32_t mc_rampingTablePos[2] = {0,0};

volatile uint32_t mc_rampingTableSteps = 0;
volatile uint32_t mc_stepsTotal[2] = {0,0};
volatile uint32_t mc_stepsTotalHalf[2] = {0,0};
volatile uint32_t mc_stepsMade[2] = {0,0};

volatile uint32_t rampingTableLenght = 0;
volatile uint32_t mc_rampingTable[500][2];

volatile int32_t mc_compensationInt[2] = {0,0};
volatile uint32_t mc_sneakEnable[2] = {0,0};
volatile uint32_t mc_sneakFlagDisable[2] = {0,0};

uint32_t mc_currentState[2];

void mc_init() {
	//pinModes einstellen
	pinMode(PIN_MR_STEP, OUTPUT);
	pinMode(PIN_MR_DIR, OUTPUT);
	pinMode(PIN_MR_ENABLE, OUTPUT);
	
	pinMode(PIN_ML_STEP, OUTPUT);
	pinMode(PIN_ML_DIR, OUTPUT);
	pinMode(PIN_ML_ENABLE, OUTPUT);
	
	digitalWrite(PIN_MR_DIR, HIGH);
	
	mc_calculateRampingTable();
	
	mc_currentState[MC_RIGHT_MOTOR] = MC_STOP;
	mc_currentState[MC_LEFT_MOTOR] = MC_STOP;

	//Interrupt initialisieren
	motor_timer.pause();
	motor_timer.setPeriod(5);
	motor_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
	motor_timer.setCompare(TIMER_CH1, 1);
	motor_timer.attachCompare1Interrupt(mc_ISR);
	motor_timer.refresh();
	motor_timer.resume();
}

void mc_calculateRampingTable(){
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

void mc_ISR(){
	// Motoren durchzählen
	for(uint8_t i = 0; i <= 1; i++){
		switch(mc_currentState[i]){
			case MC_STOP: break;
			
			case MC_RAMP_UP:
				debugVariable1 = mc_rampingTable[mc_rampingTablePos[i]][0] + mc_compensationInt[i];
				debugVariable2 = mc_rampingTablePos[i];
				// Prüfen, ob ein Step gefahren werden muss
				if(mc_delayCounter[i] > mc_rampingTable[mc_rampingTablePos[i]][0] + mc_compensationInt[i]){
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
			debugVariable1 = MC_MIN_DELAY + mc_compensationInt[i];
			debugVariable2 = mc_rampingTablePos[i];
				if(mc_delayCounter[i] > MC_MIN_DELAY + mc_compensationInt[i]){
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
			debugVariable1 = mc_rampingTable[mc_rampingTablePos[i]][0] + mc_compensationInt[i];
			debugVariable2 = mc_rampingTablePos[i];
				if(mc_delayCounter[i] > mc_rampingTable[mc_rampingTablePos[i]][0] + mc_compensationInt[i]){
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
			debugVariable1 = MC_SNEAK_DELAY + mc_compensationInt[i];
			debugVariable2 = mc_rampingTablePos[i];
				if(mc_delayCounter[i] > MC_SNEAK_DELAY + mc_compensationInt[i]){
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
		case MC_RIGHT_MOTOR:
			digitalWrite(PIN_MR_STEP, HIGH);
			digitalWrite(PIN_MR_STEP, LOW);
		break;
		case MC_LEFT_MOTOR:
			digitalWrite(PIN_ML_STEP, HIGH);
			digitalWrite(PIN_ML_STEP, LOW);
		break;
	}  
}

void mc_setCompensation(uint32_t motor, float compensation){
	switch(mc_currentState[motor]){
		case MC_COAST:
			mc_compensationInt[motor] = (float)compensation * (float)MC_MIN_DELAY;
			break;
		case MC_SNEAK:
			mc_compensationInt[motor] = (float)compensation * (float)MC_SNEAK_DELAY;
			break;
		case MC_RAMP_UP:
			mc_compensationInt[motor] = (float)compensation * (float)mc_rampingTable[mc_rampingTablePos[motor]][0];
			break;
		case MC_RAMP_DOWN:
			mc_compensationInt[motor] = (float)compensation * (float)mc_rampingTable[mc_rampingTablePos[motor]][0];
			break;
	}
}

void mc_compensate(){
	float ratio = (float)s_getLightSenor(S_LS_RM) / (float)s_getLightSenor(S_LS_LM);
	ratio = log(ratio);
	ratio = ratio / 10;
	mc_setCompensation(MC_RIGHT_MOTOR, ratio);
	mc_setCompensation(MC_LEFT_MOTOR, -ratio);
}

void mc_setSneak(uint32_t motor, bool sneak){
	if(sneak){
		mc_sneakEnable[motor] = true;
		mc_sneakFlagDisable[motor] = false;
	}
	else{
		mc_sneakEnable[motor] = false;
		mc_sneakFlagDisable[motor] = false;
	}
}

void mc_stopSneak(uint32_t motor){
	mc_sneakEnable[motor] = false;
	mc_sneakFlagDisable[motor] = true;
}

void mc_stopMotors(){
	mc_currentState[MC_LEFT_MOTOR] = MC_STOP;
	mc_currentState[MC_RIGHT_MOTOR] = MC_STOP;
}


uint32_t mc_getMotorState(uint32_t motor){
	return mc_currentState[motor];
}