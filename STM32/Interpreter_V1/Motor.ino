#define PIN_MR_STEP PB10										//Step Pin des rechten Motors
#define PIN_MR_DIR PB1											//Direction Pin des rechten Motors
#define PIN_MR_ENABLE PB11										//Enable Pin des rechten Motors

#define PIN_ML_STEP PA7											//Step Pin des linken Motors
#define PIN_ML_DIR PA6											//Direction Pin des linken Motors
#define PIN_ML_ENABLE PB0										//Enable Pin des linken Motors

uint32_t mc_minDelay = 30;										//Minimaler Delay, mit dem gefahren werden kann
uint32_t mc_sneakDelay = 300;									//Delay, auf den beim Schleichen abgebremst wird
uint32_t mc_initialDelay = 2000;								//Anfänglicher Delay in der Ramping-Delay

HardwareTimer motor_timer(2);									//Erstellen des Timers für den Motorcontroller

volatile uint32_t mc_delayCounter[2] = {0,0};					//Counter in der Motor-ISR, um den richigen Delay-Wert zu erreichen

volatile uint32_t mc_rampingTable[500][2];						//Tabelle mit Delays und Anzahl der Delays
volatile uint32_t mc_rampingTableLenght = 0;					//Anzahl der Zeilen in der Ramping-Tabelle (wie viele Delays)
volatile uint32_t mc_rampingTableSteps = 0;						//Gesamtanzahl der Steps in der Ramping-Tabelle
volatile uint32_t mc_rampingDelayCounter[2] = {0,0};			//Counter, der mitzählt, wie oft mit einem Delay ein Step gefahren wurde
volatile uint32_t mc_rampingTablePos[2] = {0,0};				//Position in der Ramping-Tabelle

volatile uint32_t mc_stepsTotal[2] = {0,0};						//Gesamtanzahl der zu fahrenden Steps
volatile uint32_t mc_stepsTotalHalf[2] = {0,0};					//Gesamtanzahl der zu fahrenden Steps halbiert
volatile uint32_t mc_stepsMade[2] = {0,0};						//Gefahrene Steps in einem move-Befehl

volatile int32_t mc_compensationInt[2] = {0,0};					//Gerundeter Kompensationswert (wird absolut auf den Delay angewandt / addiert)
volatile uint32_t mc_sneakEnable[2] = {0,0};					//Wenn true wird am Ende des RampDown-States gesneakt bis sneakFlagDisable gesetzt wird
volatile uint32_t mc_sneakFlagDisable[2] = {0,0};				//Beendet sneaken

volatile uint32_t mc_currentState[2];							//Aktueller State des rechten / linken Motors

void mc_init() {												//Initalisieren des Motorcontrollers
	//Alle Motor-Controller-Pins auf Output setzen
	pinMode(PIN_MR_STEP, OUTPUT);
	pinMode(PIN_MR_DIR, OUTPUT);
	pinMode(PIN_MR_ENABLE, OUTPUT);
	
	pinMode(PIN_ML_STEP, OUTPUT);
	pinMode(PIN_ML_DIR, OUTPUT);
	pinMode(PIN_ML_ENABLE, OUTPUT);
	
	//digitalWrite(PIN_MR_DIR, HIGH);
	
	mc_calculateRampingTable();							//Berechnen der Ramping-Tabelle
	
	mc_currentState[MC_RIGHT_MOTOR] = MC_STOP;			//State des rechten Motors initial auf STOP setzen
	mc_currentState[MC_LEFT_MOTOR] = MC_STOP;			//State des linken Motors initial auf STOP setzen

	//Timer initialisieren auf 200kHz
	motor_timer.pause();
	motor_timer.setPeriod(5);
	motor_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
	motor_timer.setCompare(TIMER_CH1, 1);
	motor_timer.attachCompare1Interrupt(mc_ISR);
	motor_timer.refresh();
	motor_timer.resume();
}

void mc_calculateRampingTable(){								//Berechnen der Ramping-Tabelle
	float delay = mc_initialDelay;												//Setzen des ersten Delay-Werts für die rekursive Berechnung
	uint32_t currentDelayInt = 0;												//Gerundete Delay-Wert
	
	mc_rampingTableLenght --;													//Kompensation zur Zuordnung Rdes ichtige Delay-Wert zur richtigen Delay-Anzahl
	while((uint16_t)delay > mc_minDelay){										//Solange der gerundete Delay-Wert nicht kleiner als der minimale Delay ist	
		if((uint16_t)delay == currentDelayInt){									//Wenn der gerundete Delay-Wert gleich dem letzten Delay-Wert ist
			mc_rampingTable[mc_rampingTableLenght][1] ++;						//Erhöhen der Delay-Anzahl
		} 
		else {																	//Ansonsten
			mc_rampingTableLenght ++;											//Erhöhen der Länge der Ramping-Tabelle
			mc_rampingTable[mc_rampingTableLenght][0] = (uint16_t)delay;		//Eintragen des neuen Delay-Werts
			mc_rampingTable[mc_rampingTableLenght][1] ++;						//Erhöhen der Delay-Anzahl
			currentDelayInt = (uint16_t)delay;									//Setzen des neuen gerundeten Delay-Werts									
		}
		
		mc_rampingTableSteps++;													//Erhöhen der Gesamtsteps der Ramping-Tabelle
		delay = delay - (2 * delay) / (4 * mc_rampingTableSteps + 1);			//Berechnen des neuen Delay-Werts	
	}
	mc_rampingTableLenght++;													//Kompensation zur Zuordnung Rdes ichtige Delay-Wert zur richtigen Delay-Anzahl
}

void mc_ISR(){													//Motor ISR: Setzt den nächsten Delay für die Stepper
	for(uint8_t i = 0; i <= 1; i++){																				//For-Schleife um beide Motoren zu bearbeiten
		switch(mc_currentState[i]){																					//Switch-Case um den aktuellen Stand zu behandeln
			
			case MC_STOP: 																							//Stop: Nichts tun
				break;
			
			case MC_RAMP_UP:																						//Hochbeschleunigen
				if(mc_delayCounter[i] > mc_rampingTable[mc_rampingTablePos[i]][0] + mc_compensationInt[i]){ 		//Prüfen, ob genug Zeit seit dem Letzten Step vergangen ist und somit ein Step gefahren werden muss
					
					mc_motorStep(i);																				//Einen Step fahren
					mc_stepsMade[i]++;																				//Anzahl der gefahrenen Steps erhöhen
					mc_rampingDelayCounter[i] ++;																	//Anzahl der schon gefahrenen Steps für diesen Delay
					
					if(mc_rampingDelayCounter[i] >= mc_rampingTable[mc_rampingTablePos[i]][1]){						//Prüfen, ob der nächste Delay ausgewählt werden muss	
						mc_rampingTablePos[i] ++;																	//Position in der Ramping-Tabelle erhöhen
						mc_rampingDelayCounter[i] = 0;																//Counter für die Anzahl der gefahrenen Steps pro Delay wieder auf 0 setzen
						
						if(mc_stepsMade[i] == mc_rampingTableSteps){												//Falls die Ramping-Tabelle komplett gefahren wurde
							mc_currentState[i] = MC_COAST;															//Übergehen in den Coast State
						}
					}				
					
					if(mc_stepsMade[i] == mc_stepsTotalHalf[i]){													//Falls die bisher gemachten Steps bereits die hälfte der zu machenden Steps sind
						mc_currentState[i] = MC_RAMP_DOWN;															//Übergehen in den Brems-State
						mc_rampingDelayCounter[i] ++;																//Erhöhen des Counters für die Anzahl der gefahrenen Steps pro Delay damit man beim Bremsen auf null kommt
					
					}
					
					mc_delayCounter[i] = 0;																			//Resetten des Delay-Counters
				}
				mc_delayCounter[i] ++;																				//Erhöhen des Delay-Counters
				break;
				
			case MC_COAST:																							//Maximale Geschwindigkeit halten
				if(mc_delayCounter[i] > mc_minDelay + mc_compensationInt[i]){										//Prüfen, ob genug Zeit seit dem Letzten Step vergangen ist und somit ein Step gefahren werden muss
					
					mc_motorStep(i);																				//Einen Step fahren
					mc_stepsMade[i]++;																				//Anzahl der gefahrenen Steps erhöhen
					
					if(mc_stepsMade[i] >= (mc_stepsTotal[i] - mc_rampingTableSteps)){								//Prüfen ob noch genug Steps zu fahren sind um abzubremsen
						mc_currentState[i] = MC_RAMP_DOWN;															//Übergehen in den Brems State
					}
					
					mc_delayCounter[i] = 0;																			//Resetten des Delay-Counters
				}
				mc_delayCounter[i] ++;																				//Erhöhen des Delay-Counters
				break;
				
			case MC_RAMP_DOWN:																						//Abbremsen
				if(mc_delayCounter[i] > mc_rampingTable[mc_rampingTablePos[i]][0] + mc_compensationInt[i]){			//Prüfen, ob genug Zeit seit dem Letzten Step vergangen ist und somit ein Step gefahren werden muss
					
					mc_motorStep(i);																				//Einen Step fahren
					mc_stepsMade[i]++;																				//Anzahl der gefahrenen Steps erhöhen
					
					if(mc_rampingDelayCounter[i] == 0){																//Prüfen ob alles Steps mit dem jetzigen Delay gefahren worden sind
						mc_rampingTablePos[i] --;																	//In der Raping Tabelle einen Schritt nach oben machen
						mc_rampingDelayCounter[i] = mc_rampingTable[mc_rampingTablePos[i]][1];						//Counter für die Anzahl der gefahrenen Steps pro Delay wieder auf den richtigen Wert setzen
						
						if(mc_sneakEnable[i] && mc_rampingTable[mc_rampingTablePos[i]][0] >= mc_sneakDelay){		//Prüfen ob Sneaken aktiviert ist und der jetzige Delay größer als der Sneak-Delay ist
							mc_currentState[i] = MC_SNEAK;															//Übergehen in den Sneak-State
						}
						else if(mc_stepsMade[i] >= mc_stepsTotal[i]){												//Prüfen ob alle Steps gefahren wurden
							mc_currentState[i] = MC_STOP;															//Übergehen in den Stop-State
						}
					}	
					
					mc_rampingDelayCounter[i] --;																	//Anzahl der gefahrenen Steps für diesen Delay um eins verringern			
					mc_delayCounter[i] = 0;																			//Resetten des Delay-Counters
				}
				mc_delayCounter[i] ++;																				//Erhöhen des Delay-Counters
				break;
				
			case MC_SNEAK:																							//Sneaken
				if(mc_delayCounter[i] > mc_sneakDelay + mc_compensationInt[i]){									//Prüfen, ob genug Zeit seit dem Letzten Step vergangen ist und somit ein Step gefahren werden muss
					
					if(mc_sneakFlagDisable[i]){																		//Prüfen ob die Sneak-Disable-Flag gesetzt wurde
						mc_sneakFlagDisable[i] = false;																//Die Sneak-Disable-Flag wieder auf false resetten
						mc_currentState[i] = MC_STOP;																//Übergehen in den Stop-State
						break;																						//Case verlassen
					}
					
					mc_motorStep(i);																				//Einen Step fahren
					mc_delayCounter[i] = 0;																			//Resetten des Delay-Counters
				}
				mc_delayCounter[i] ++;																				//Erhöhen des Delay-Counters
				break;
		}
	}	
}

void mc_move(uint8_t motor, int32_t steps){						//Gibt den Befehl einen Motor um die angegebenen Steps zu bewegen
	if(steps < 0){										//Wenn Steps negativ ist
		steps = -steps;									//steps wieder positv machen
		
		switch(motor){									//Motordrehrichtung einstellen
			case MC_LEFT_MOTOR:
				digitalWrite(PIN_ML_DIR, HIGH);
				break;
			case MC_RIGHT_MOTOR:
				digitalWrite(PIN_MR_DIR, LOW);
				break;
		}
	
	} else 	{											//Wenn Steps positiv ist
		switch(motor){									//Motordrehrichtung einstellen
			case MC_LEFT_MOTOR:
				digitalWrite(PIN_ML_DIR, LOW);
				break;
			case MC_RIGHT_MOTOR:
				digitalWrite(PIN_MR_DIR, HIGH);
				break;
		}
	}
	mc_delayCounter[motor] = 0;							//Variablen für den Motorcontroller resetten
	mc_rampingTablePos[motor] = 0;
	mc_rampingDelayCounter[motor] = 0;
	mc_stepsMade[motor] = 0;
	mc_stepsTotal[motor] = steps;	
	mc_stepsTotalHalf[motor] = steps / 2;	
	mc_currentState[motor] = MC_RAMP_UP;				//In den RampUp-State gehen
}

void mc_motorStep(uint8_t motor){								//Gibt einen Puls an den entsprechenden Motortreiber
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

void mc_setCompensation(uint8_t motor, float compensation){		//Setzt die Kompensation für die Motoren entsprechend des aktuellen States
	switch(mc_currentState[motor]){
		case MC_COAST:
			mc_compensationInt[motor] = (float)compensation * (float)mc_minDelay;
			break;
		case MC_SNEAK:
			mc_compensationInt[motor] = (float)compensation * (float)mc_sneakDelay;
			break;
		case MC_RAMP_UP:
			mc_compensationInt[motor] = (float)compensation * (float)mc_rampingTable[mc_rampingTablePos[motor]][0];
			break;
		case MC_RAMP_DOWN:
			mc_compensationInt[motor] = (float)compensation * (float)mc_rampingTable[mc_rampingTablePos[motor]][0];
			break;
	}
}

void mc_compensate(){											//Berechnet den aktuellen Kompensationswert
	float ratio = (float)sn_getLightSenor(S_LS_RM) / (float)sn_getLightSenor(S_LS_LM);		//Berechnet das Verhältniss zwichen den beiden Sensoren (ca: 1/20 bis 20)
	ratio = log(ratio);																		//Wendet den Logarithmus auf das Verhältniss an (ca: -2 bis 2)
	ratio = ratio / 10;																		//Teilt durch einen Faktor von 10 um kleinere Werte zu bekommen, bestimmt die aggresivität des Lininenfolgens
	mc_setCompensation(MC_RIGHT_MOTOR, ratio);												//Kompensation für den rechten Motor setzten
	mc_setCompensation(MC_LEFT_MOTOR, -ratio);												//Kompensation für den linken Motor setzen
}

void mc_resetCompensation(){									//Setzt die Kompensation wieder auf null
	mc_compensationInt[MC_LEFT_MOTOR] = 0;
	mc_compensationInt[MC_RIGHT_MOTOR] = 0;
}

void mc_setSneak(uint32_t motor, bool sneak){					//Aktiviert/Deaktiviert Sneaken für den jeweiligen Motor
	if(sneak){
		mc_sneakEnable[motor] = true;
		mc_sneakFlagDisable[motor] = false;
	}
	else{
		mc_sneakEnable[motor] = false;
		mc_sneakFlagDisable[motor] = false;
	}
}

void mc_stopSneak(uint32_t motor){								//Beendet Sneaken
	mc_sneakEnable[motor] = false;
	mc_sneakFlagDisable[motor] = true;
}

uint32_t mc_getMotorState(uint32_t motor){ 						//Gibt den aktuellen State des Motors
	return mc_currentState[motor];
}

uint32_t mc_getSteps(uint32_t motor){							//Gibt die bereits gefahrenen Steps des Motors
	return mc_stepsMade[motor];
}

void mc_stopMotors(){
	mc_currentState[0] = MC_STOP;
	mc_currentState[1] = MC_STOP;
}

void mc_setSpeedValues(uint32_t init, uint32_t min, uint32_t sneak){
	mc_initialDelay = init;
	mc_minDelay = min;
	mc_sneakDelay = sneak;
	

	for(uint32_t i = 0; i < 500; i++){
		mc_rampingTable[i][0] = 0;
		mc_rampingTable[i][1] = 0;
	}
	mc_rampingTableLenght = 0;
	mc_rampingTableSteps = 0;
	
	mc_calculateRampingTable();
}