#define I_MM_TO_STEPS_FAKTOR (50/3)						//mm in steps -> mm * 50/3

#define I_LS_THRESHOLD 2000								//Wert des Helligkeitsensors ab dem Schwarz erkannt werden soll
#define I_D_THRESHOLD 700								//Wert des Entfernungssensors ab dem ein Obstacle erkannt wird
#define I_HL_THRESHOLD_MIN 1500							//Wert des Hallsensors, ab dem der Min-Wert erkannt wird
#define I_HL_THRESHOLD_MAX 2500							//Wert des Hallsensors, ab dem der Max-Wert erkannt wird
#define I_HL_MIDVALUE 2100

#define I_TURN_STEPS_OUTER 2440							//Steps die der äußere Motor fährt
#define I_TURN_STEPS_INNER 670							//Steps die der innere Motor fährt
#define I_TURNSTEPS_180 3020
#define I_TURNSTEPS 1500								//Steps die nach einer Kurve abgezogen werden
#define I_SNEAKSTEPS 500

#define I_LAUNCH_STEPS 4500								//Steps für die Anfangsphase mit Bitfeldern
#define I_MAX_VALUE_ARRAY_LENGTH 50

uint8_t i_currentRoute = I_ROUTE_A;						//Route die gefahren werden muss um dem nächsten Hinderniss auszuweichen
uint8_t i_packetAddress[3] = {0, 0, 0};					//gelesene Adressen: 0 -> grün, 1-> gelb, 2 -> rot
int8_t i_packetPriority[3] = {0, 0, 0};
uint8_t i_countBitsRead = 100;							//Anzahl der schon gelesenen Bits (auf Hundert um die erste Linie zu ignorieren)
uint8_t i_stateOrder_pos = 0;							//Position im Statearray des Aktuellen Levels
uint8_t i_state = I_WAIT_BUTTON;						//Aktueller state (initial: auf Knopfdruck warten)
uint8_t i_initialiser = true;							//Boolean um States zu initialisieren


uint8_t i_preparedLvl1 = false;
uint8_t i_preparedLvl2 = false;

uint32_t i_delayCounter = 0;

int i_stateOrder[I_MAX_VALUE_ARRAY_LENGTH][5];

//Reihenfolge und Argument der States der Level 1 und 2
//Argument 0: State
//Argument 1: bei I_DRIVE_INTERSECTION: Anzahl der zu fahrenden Steps. Sonst: undefiniert
//Argument 2: Routenzuordnung (COMMON gilt für beide Routen, A ist die jeweils äußere, B die innere)
//Argument 3: Abladefeld und Richtung (Positv heißt rechts ist das Referenzbitfeld)
//Argument 4: mm bis zum Abladen
int32_t i_stateOrder_lvl_1[][5] = {
	{I_WAIT_BUTTON,			0,							I_ROUTE_COMMON,     0},
	{I_LAUNCH,				0,							I_ROUTE_COMMON,		0},	
	{I_PREPARE,				0,							I_ROUTE_COMMON,		5},
	
	//Hinderniss 0
	{I_READ_OBSTACLE, 		0,							I_ROUTE_COMMON,		0},	
	{I_DRIVE_INTERSECTION, 	330 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_A,			0},	
	{I_TURN_RIGHT, 			0,		 					I_ROUTE_B,			0},
	{I_DRIVE_INTERSECTION, 	155 * I_MM_TO_STEPS_FAKTOR, I_ROUTE_B,			0},
	{I_TURN_LEFT, 			0,							I_ROUTE_B,			0},	
	{I_DRIVE_INTERSECTION, 	330 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_B,			0},
	{I_TURN_LEFT, 			0,							I_ROUTE_B,			0},	
	{I_DRIVE_INTERSECTION, 	155 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_B,			0},
	{I_TURN_RIGHT, 			0, 							I_ROUTE_B,			0},
	
	{I_DRIVE_INTERSECTION, 	315 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_A,		-5,		180 * I_MM_TO_STEPS_FAKTOR},
	{I_DRIVE_INTERSECTION, 	315 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_B,		-5,		150 * I_MM_TO_STEPS_FAKTOR},
	{I_PREPARE,				0,							I_ROUTE_COMMON,		1},
	{I_WAIT,				2000,						I_ROUTE_COMMON,		0},
	{I_TURN_RIGHT,			0,							I_ROUTE_COMMON,		0},
	
	{I_DRIVE_INTERSECTION, 	350 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		1,		160 * I_MM_TO_STEPS_FAKTOR},
	{I_PREPARE,				0,							I_ROUTE_COMMON,		2},
	
	//Hinderniss 1
	{I_READ_OBSTACLE, 		1,							I_ROUTE_COMMON,		0},	
	{I_DRIVE_INTERSECTION, 	205 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_A,			0},	
	{I_TURN_RIGHT,			0,							I_ROUTE_A,			0},
	{I_DRIVE_INTERSECTION, 	155 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_A,			0},	
	{I_TURN_RIGHT,			0,							I_ROUTE_B,			0},
	{I_DRIVE_INTERSECTION, 	155 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_B,			0},
	{I_TURN_LEFT,			0,							I_ROUTE_B,			0},
	{I_DRIVE_INTERSECTION, 	205 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_B,			0},
	{I_TURN_RIGHT,			0,							I_ROUTE_B,			0},
	
	{I_DRIVE_INTERSECTION, 	335 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_A,			2,		110 * I_MM_TO_STEPS_FAKTOR},
	{I_DRIVE_INTERSECTION, 	335 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_B,			2,		75 * I_MM_TO_STEPS_FAKTOR},
	{I_PREPARE,				0,							I_ROUTE_COMMON,		3},
	{I_WAIT,				700,						I_ROUTE_B,			0},
	{I_TURN_RIGHT,			0,							I_ROUTE_COMMON,		0},
	
	{I_DRIVE_INTERSECTION, 	205 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		3,		100 * I_MM_TO_STEPS_FAKTOR},
	{I_PREPARE,				0,							I_ROUTE_COMMON,		4},
	{I_TURN_LEFT,			0,							I_ROUTE_COMMON,		0},
	{I_DRIVE_INTERSECTION, 	155 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		0},
	{I_TURN_LEFT,			0,							I_ROUTE_COMMON,		0},
	{I_DRIVE_INTERSECTION,	205 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		0},
	{I_TURN_RIGHT,			0,							I_ROUTE_COMMON,		0},
	
	//Hinderniss 2
	{I_DRIVE_INTERSECTION, 	155 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		0},
	{I_READ_OBSTACLE, 		2,							I_ROUTE_COMMON,		0},	
	{I_TURN_RIGHT,			0,							I_ROUTE_B,			0},	
	{I_DRIVE_INTERSECTION, 	155 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		0},	
	{I_TURN_LEFT,			0,							I_ROUTE_B,			0},	
	{I_TURN_RIGHT,			0,							I_ROUTE_A,			0},	
	{I_DRIVE_INTERSECTION, 	155 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		0},	
	{I_TURN_RIGHT,			0,							I_ROUTE_B,			0},	
		
	{I_DRIVE_INTERSECTION, 	245 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		-4,		290 * I_MM_TO_STEPS_FAKTOR},
	{I_VOID,				0,							I_ROUTE_COMMON,		0}
};

int32_t i_stateOrder_lvl_2_begin[][5] = {
	{I_WAIT_BUTTON,			0,							I_ROUTE_COMMON,     0},
	{I_LAUNCH,				0,							I_ROUTE_COMMON,		0},
	{I_DRIVE_INTERSECTION,	645 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		0},
	{I_TURN_RIGHT,			0,							I_ROUTE_COMMON,		0},
	{I_DRIVE_INTERSECTION,	350 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		0},
	{I_SET_FIRST_ROUTE,		0,							I_ROUTE_COMMON,		0}
};

void i_init(uint8_t level){	
																					//Initialisierung des Interpreters
	mc_stopMotors();
	sv_initServos();
	
	i_packetAddress[0] = 0;
	i_packetAddress[1] = 0;
	i_packetAddress[2] = 0;
	i_countBitsRead = 100;
		
	i_packetPriority[0] = 0;
	i_packetPriority[1] = 0;
	i_packetPriority[2] = 0;

	
	if(level == 1){
		for(int y = 0; y < I_MAX_VALUE_ARRAY_LENGTH; y++){
			for(int x = 0; x < 5; x++){
				i_stateOrder[y][x] = i_stateOrder_lvl_1[y][x];
			}
		}		
	}
	
	if(level == 2){
		for(int y = 0; y < I_MAX_VALUE_ARRAY_LENGTH; y++){
			for(int x = 0; x < 5; x++){
				i_stateOrder[y][x] = i_stateOrder_lvl_2_begin[y][x];
			}
		}
	}
	
	
	i_stateOrder_pos = 0;
	i_state = i_stateOrder[0][0];
}

void i_loop(){																								//Hauptloop des interpreters
	prepareLevel();

	switch(i_state){									//Impementierung des State-Systems
		case I_LAUNCH:
			i_launch();
			break;
			
		case I_WAIT_BUTTON:
			i_waitButton();
			break;
			
		case I_READ_OBSTACLE:
			i_readObstacle();
			break;
			
		case I_DRIVE_INTERSECTION:
			i_driveToIntersection();
			break;
			
		case I_TURN_RIGHT:
			i_turnRight();
			break;
			
		case I_TURN_LEFT:
			i_turnLeft();
			break;
		
		case I_TURN_180:
			i_turn180();
			break;
		
		case I_VOID:
			break;
			
		case I_SET_FIRST_ROUTE:
			//i_setFirstRoute();
			break;
			
		case I_SET_SECOND_ROUTE:
			//i_setSecondRoute();			
			break;
		
		case I_PREPARE:
			i_preparePackage();			
			break;
			
		case I_WAIT:
			i_wait();			
			break;
	}
	
}

void i_nextState(){				
	i_initialiser = true;																					//Initialiser wieder zurücksetzen
	i_stateOrder_pos ++;																					///State hochzählen
	
	while(i_currentRoute == I_ROUTE_A && i_stateOrder[i_stateOrder_pos][2] == I_ROUTE_B){				//States überspringen, die nicht ausgeführt werden müssen ( B überspringen)
		i_stateOrder_pos++;
	}
	
	while(i_currentRoute == I_ROUTE_B && i_stateOrder[i_stateOrder_pos][2] == I_ROUTE_A){				//States überspringen, die nicht ausgeführt werden müssen ( A überspringen)
		i_stateOrder_pos++;
	}
	
	i_state = i_stateOrder[i_stateOrder_pos][0];															//neuen State setzen
}

void prepareLevel(){
	if(!sn_getButton(S_BUTTON_1) && !i_preparedLvl1){
		i_preparedLvl2 = false;
		i_preparedLvl1 = true;
		i_init(1);
		delay(500);
	}
	if(!sn_getButton(S_BUTTON_2) && !i_preparedLvl2){
		i_preparedLvl1 = false;
		i_preparedLvl2 = true;
		i_init(2);
		delay(500);
	}
}

void i_waitButton(){
	static uint8_t buttonPressed1 = false;
	if(!sn_getButton(S_BUTTON_1) && i_preparedLvl1){
		buttonPressed1 = true;
		delay(15);
	}
	if(sn_getButton(S_BUTTON_1) && buttonPressed1){
		buttonPressed1 = false;
		i_preparedLvl1 = false;
		i_nextState();
	}
	
	static uint8_t buttonPressed2 = false;
	if(!sn_getButton(S_BUTTON_2) && i_preparedLvl2){
		buttonPressed2 = true;
		delay(15);
	}
	if(sn_getButton(S_BUTTON_2) && buttonPressed2){
		buttonPressed2 = false;
		i_preparedLvl2 = false;
		i_nextState();
	}
}

void i_launch(){																							//Erster State um die Bifelder am Anfang auszulesen                                                

	if(i_initialiser){									//Initialiser wird einmal am Anfang des States aufgerufen
		i_initialiser = false;							//Initialiser wird damit das nächste Mal nicht mehr aufgerufen
		mc_move(MC_LEFT_MOTOR, I_LAUNCH_STEPS);			//Motoren bis zur Kreuzung fahren lassen
		mc_setSneak(MC_LEFT_MOTOR, true);
		mc_move(MC_RIGHT_MOTOR, I_LAUNCH_STEPS);	
		mc_setSneak(MC_RIGHT_MOTOR, true);
	}
	
	//Bifelder (Schwarz/Weiß-Felder und Magnetfelder) lesen
	static uint8_t light_lastValue = 0;								//letzter gemessener Wert des rechten Bitfelds
	static uint32_t light_maxInBitField = 0;						//maximal gemessener Wert in einem Bit
	
	static uint32_t hall_maxInBitField = I_HL_MIDVALUE;
	static uint32_t hall_minInBitField = I_HL_MIDVALUE;
	
	uint16_t light_right = sn_getLightSenor(S_LS_RR);					//Auslesen des rechten Sensors zum erkennen der Referenz-Bitlfelder
	uint16_t light_left = sn_getLightSenor(S_LS_LL);					//Auslesen des linken Sensors zum erkennen der Daten-Bitlfelder
	
	
	//Hallsensor auslesen
		//Positionen für Magnete: 2,5,8
	uint16_t hall = sn_getHallSensor();
	if(hall > hall_maxInBitField){
		hall_maxInBitField = hall;
	}
	if(hall < hall_minInBitField){
		hall_minInBitField = hall;
	}
		
	//Lichtsensor auslesen
	if(light_left > light_maxInBitField){									//Maximalen Wert von LL merken
		light_maxInBitField = light_left;									
	}	
	
	if(!light_lastValue && light_right > I_LS_THRESHOLD){					//Steigende Flanke an RR
		light_lastValue = true;
		light_maxInBitField = 0;										//max-Wert zurücksetzen
	}
	
	else if(light_lastValue && light_right < I_LS_THRESHOLD){				//Fallende Flanke an RR
		light_lastValue = false;
		
		if(i_countBitsRead == 100){								//Ignorieren der ersten Linie durch Workaround
			i_countBitsRead = 0;
			return;
		}
		
		i_packetAddress[i_countBitsRead / 3] <<= 1;				//Adresse nach Links shiften
		if(light_maxInBitField > I_LS_THRESHOLD){						//Wenn wert schwarz war, dann Bit auf 1 setzen
			i_packetAddress[i_countBitsRead / 3] |= 1;			
		}
		
		
		//Hallsensorwerte in diskrete Werte umwandeln
		int8_t hl_curr_value = 0;
		if(hall_maxInBitField > I_HL_THRESHOLD_MAX){
				hl_curr_value = 1;
		}
		else if (hall_minInBitField < I_HL_THRESHOLD_MIN){
				hl_curr_value = -1;
		}
			
		//Diskrete Werte speichern
		if(i_countBitsRead == 2){
			i_packetPriority[0] = hl_curr_value;
		}
		else if(i_countBitsRead == 5){
			i_packetPriority[1] = hl_curr_value;
		}
		else if(i_countBitsRead == 8){
			i_packetPriority[2] = hl_curr_value;
		}
		
		i_countBitsRead++;
		
		//Hallsensor zurücksetzen
		hall_maxInBitField = I_HL_MIDVALUE;
		hall_minInBitField = I_HL_MIDVALUE;
	}
	
	db_setRgbLed(light_lastValue, light_lastValue, light_lastValue);
	
	mc_compensate();											//Linienfolgen
	
	if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){			//Abbruchbedingung
		if(sn_getLightSenor(S_LS_RR) > I_LS_THRESHOLD){
			mc_stopSneak(MC_LEFT_MOTOR);						//Motoren anhalten
			mc_stopSneak(MC_RIGHT_MOTOR);
			mc_resetCompensation();								//Compensation auf 0 setzen
			
			Serial.print(i_packetAddress[0]);
			Serial.print(", ");
			Serial.print(i_packetAddress[1]);
			Serial.print(", ");
			Serial.println(i_packetAddress[2]);
			
			Serial.print(i_packetPriority[0]);
			Serial.print(", ");
			Serial.print(i_packetPriority[1]);
			Serial.print(", ");
			Serial.println(i_packetPriority[2]);
			
			Serial.println();
		
			i_nextState();										//In den nächsten State gehen
		}
	}
	
}

void i_turnLeft(){																							//Nach links abbiegen
	if(i_initialiser){																						//Diesen Teil nur einmal aufrufen
		i_initialiser = false;
		mc_move(MC_RIGHT_MOTOR, I_TURN_STEPS_OUTER);														//Motoren bewegen
		mc_move(MC_LEFT_MOTOR, -I_TURN_STEPS_INNER);	
	}
	
	if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_STOP ){														//Abbruchbedingung: Wenn der rechte Motor steht
		i_nextState();																						//Nächsten State aufrufen
	}
}

void i_turnRight(){
	if(i_initialiser){																						//Diesen Teil nur einmal aufrufen
		i_initialiser = false;																				
		mc_move(MC_LEFT_MOTOR, I_TURN_STEPS_OUTER);															//Motoren bewegen
		mc_move(MC_RIGHT_MOTOR, -I_TURN_STEPS_INNER);
	}
	if(mc_getMotorState(MC_LEFT_MOTOR) == MC_STOP ){														//Abbruchbedingung: Wenn der linke	Motor steht
		i_nextState();																						//Nächsten State aufrufen
	}
}

void i_turn180(){
	if(i_initialiser){																						//Diesen Teil nur einmal aufrufen
		i_initialiser = false;																				
		mc_move(MC_LEFT_MOTOR, I_TURNSTEPS_180);															//Motoren bewegen
		mc_move(MC_RIGHT_MOTOR, -I_TURNSTEPS_180);
	}
	if(mc_getMotorState(MC_LEFT_MOTOR) == MC_STOP ){														//Abbruchbedingung: Wenn der linke	Motor steht
		i_nextState();																						//Nächsten State aufrufen
	}
}

uint32_t i_getLastRelevantStatePos(uint32_t referenceStatePos, uint32_t currentRoute){
	uint32_t statesBack = 1;
	while(i_stateOrder[referenceStatePos - statesBack][2] == I_ROUTE_A && currentRoute == I_ROUTE_B || 
		  i_stateOrder[referenceStatePos - statesBack][2] == I_ROUTE_B && currentRoute == I_ROUTE_A){
			  
		statesBack++;
	}
	return referenceStatePos - statesBack;
}

void i_driveToIntersection(){		
	static uint8_t armCentered = false;
																		//Bis zur nächsten Kreuzung fahren
	if(i_initialiser){																									//Initialiser wird einmal am Anfang aufgerufen
		i_initialiser = false;																							//Damit wird der Initialiser das nächste mal nicht mehr aufgeführt
			
		armCentered = false;
			
		//---------------------------------------------------
		//		ABZIEHEN DER TURNSTEPS (FALLS NOTWENDIG)
		//---------------------------------------------------
		uint32_t steps = i_stateOrder[i_stateOrder_pos][1];															//Steps fürs erste aus der state_order auslesen		
		uint32_t lastRelevantState = i_stateOrder[i_getLastRelevantStatePos(i_stateOrder_pos, i_currentRoute)][0];	//Einen relevanten Schritt zurück schauen, und  prüfen, ob der letzte Schritt ein Turn war
		
		
		if(lastRelevantState == I_TURN_LEFT || lastRelevantState == I_TURN_RIGHT || lastRelevantState == I_TURN_180){
			steps -= I_TURNSTEPS;																						//Wenn ja, Turnsteps abziehen
		}
		
		
		//Prepare Arm
		if(i_stateOrder[i_stateOrder_pos][3] != 0){
			sv_lowerArm(i_stateOrder[i_stateOrder_pos][3]);
		}
		
		
		//---------------------------------------------------
		//				FAHRBEFEHL GEBEN
		//---------------------------------------------------
		mc_move(MC_LEFT_MOTOR, steps);																					//Linken Motor fahren
		mc_setSneak(MC_LEFT_MOTOR, true);																				//Sneaken auf true setzen
		mc_move(MC_RIGHT_MOTOR, steps);																					//Rechten Motor fahren
		mc_setSneak(MC_RIGHT_MOTOR, true);																				//Sneaken auf true setzen
	}
	
	
	
	// TODO : handeln was passiert wenn der RR sensor schwarz hat aber noch gefahren werden muss
	//			damit kein zucken ins fahren kommt	
	
	
	//-----------------------------------------------------------------------------
	//						LINIENFOLGEN
	//-----------------------------------------------------------------------------
	mc_compensate();
	
	
	
	//-----------------------------------------------------------------------------
	//						PAKETAUSLIEFERUNG
	//-----------------------------------------------------------------------------
	
	
	
	
	//Steps zählen für Packetauslieferung
	if(i_stateOrder[i_stateOrder_pos][3] != 0){
		if(mc_getSteps(MC_RIGHT_MOTOR) > i_stateOrder[i_stateOrder_pos][4]){
			if(armCentered == false){
				armCentered = true;
				sv_centerArm();
			}
		}
	}
	
	//-----------------------------------------------------------------------------
	//						ABBRUCHBEDINGUNG
	//-----------------------------------------------------------------------------
	if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){														//WEnn der Motor schon am Sneaken ist
		if(sn_getLightSenor(S_LS_RR) > I_LS_THRESHOLD || sn_getLightSenor(S_LS_LL) > I_LS_THRESHOLD){		//und der Lichtsensor außen die Kreuzung erkennt
			mc_stopSneak(MC_LEFT_MOTOR);																	//Beide Motoren anhalten
			mc_stopSneak(MC_RIGHT_MOTOR);
			mc_resetCompensation();																			//Kompensation zurücksetzen
			i_nextState();																					//Nächsten State aufrufen
		} 			
	}
}

void i_preparePackage(){
	uint8_t adress = i_stateOrder[i_stateOrder_pos][3];
	for(uint8_t i = 0; i < 3; i++){
		if(i_packetAddress[i] == adress){
			sv_loadArm(i);
			Serial.print("load: ");
			Serial.println(i);
			break;
		}
	}
	i_nextState();
}

void i_readObstacle(){																						//Erkennen eines Hindernisses und dementsprechend i_currentRoute und i_obstaclePositions setzen	
	if(sn_getDistance() > I_D_THRESHOLD){								//i_obstaclePositions an dieser Stelle auf false setzen																						// -> Die Position im Array wird aus dem State ausgelesen
		i_currentRoute = I_ROUTE_B;																			//Route auf B setzen
	}
	else {																								//i_obstaclePositions an dieser Stelle auf false setzen
		i_currentRoute = I_ROUTE_A;																			//Route auf A setzen
	}
	
	i_nextState();																							//Nächsten State aufrufen
}

void i_wait(){
	if(i_initialiser){																									//Initialiser wird einmal am Anfang aufgerufen
		i_initialiser = false;
		i_delayCounter = 0;
	}
	
	if(i_delayCounter >= i_stateOrder[i_stateOrder_pos][1]){
		i_nextState();
	}
	
}




