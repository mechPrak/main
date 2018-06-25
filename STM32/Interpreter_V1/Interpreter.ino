/*
TODO:
	- differenzieren zwischen Level 1 und 2 (mit i_currentLevel) -> variable ob lvl 1 schon gefahren wurde
	- kalibrieren des Frontsensors an erste Linienfolgen
*/

#define I_THRESHOLD 2000							//Wert des Helligkeitsensors ab dem Schwarz erkannt werden soll

#define I_TURN_STEPS_OUTER 2350							//Steps um eine Kurve zu fahren
#define I_TURN_STEPS_INNER 700

#define I_LAUNCH_STEPS 3500								//Steps für die Anfangsphase mit Bitfeldern
#define I_BIT_READ_STEPDELAY 150						//Steps die nach der steigenden Flanke noch gewartet werden bevor Bit gelesen wird


//Reihenfolge und Argument der States der Level 1 und 2
uint32_t i_stateOrder_l1[50][2] = {
      {I_DRIVE_INTERSECTION, 15000},
      {I_TURN_RIGHT},
      {I_DRIVE_INTERSECTION, 4000},
      {I_TURN_RIGHT},
      {I_DRIVE_INTERSECTION, 750},
      {I_TURN_LEFT},
      {I_DRIVE_INTERSECTION, 1500},
      {I_TURN_RIGHT},
      {I_DRIVE INTERSECTION, 4000},
      {I_TURN_RIGHT},
      {I_DRIVE_INTERSECTION, 1500},
      {I_TURN_LEFT},
      {I_DRIVE_INTERSECTION, 750},
      {I_TURN_LEFT},
      {I_DRIVE_INTERSECTION, 1500},
      {I_TURN_RIGHT},
      {I_DRIVE_INTERSECTION, 4000},
      {I_TURN_RIGHT},
      {I_DRIVE_INTERSECTION, 4500},
      {I_TURN_RIGHT},
      {I_DRIVE_INTERSECTION, 4000},
      {I_TURN_LEFT},
      {I_DRIVE_INTERSECTION,750},
      {I_TURN_LEFT},
      {I_DRIVE_INTERSECTION, 4000}
      {I_WAIT_BUTTON}
    };
uint32_t i_stateOrder_l2[50][2];						//Arraybedeutung: 0 -> State, 1 -> Argument für State

uint8_t i_currentLevel = 0;								//Aktuelles Level
uint8_t i_stateOrder_pos = 0;							//Position im Statearray des Aktuellen Levels
uint8_t i_state = I_WAIT_BUTTON;						//Aktueller state (initial: auf Knopfdruck warten)
uint8_t i_initialiser = true;							//Boolean um States zu initialisieren
uint8_t i_wait = false;									//Boolean um in States einen Wartezustand zu ermöglchen
uint32_t i_counter = 0;	

//Bitfelder
uint8_t i_packetAddress[3] = {0, 0, 0};					//gelesene Adressen: 0 -> grün, 1-> gelb, 2 -> rot


void i_init(){
	i_currentLevel = 1;
		
	i_state = i_stateOrder_l1[0][0];
	
}

void i_loop(){
	switch(i_state){
		case I_LAUNCH:
			//Debug
			//Serial.println("Launch");
			
			i_launch();
			break;
			
		case I_WAIT_BUTTON:
			//Debug
			Serial.println("Wait for button");
			
			//TODO: IMPLEMENT	
			i_packetAddress[0] = 0;
			i_packetAddress[1] = 0;
			i_packetAddress[2] = 0;
			delay(3000);
			i_reset();
		
			break;
			
		case I_DRIVE_INTERSECTION:
			//Debug
			Serial.println("Drive to intersection");
			
			i_driveToIntersection();
			break;
			
		case I_TURN_RIGHT:
			//Debug
			Serial.println("Turn right");
			
			i_turnRight();
			break;
			
		case I_TURN_LEFT:
			//Debug
			Serial.println("Turn left");
			
			i_turnLeft();
			break;
	}
}


/*
i_launch: 	Abfahrt aus dem Startfeld
			Kalibrieren der Helligkeitssensoren an erster Linie
			Lesen der Anfangsbitfelder und Speichern der Werte
*/
void i_launch(){
	if(i_initialiser){
		i_initialiser = false;
		
		mc_move(MC_LEFT_MOTOR, I_LAUNCH_STEPS);
		mc_setSneak(MC_LEFT_MOTOR, true);
		
		mc_move(MC_RIGHT_MOTOR, I_LAUNCH_STEPS);	
		mc_setSneak(MC_RIGHT_MOTOR, true);
	}
	
	//Bifelder lesen
	static uint8_t lastValue = 0;				//letzter gemessener Wert des rechten Bitfelds
	static uint32_t maxInBitField = 0;			//maximal gemessener Wert in einem Bit
	static uint8_t countBitsRead = 100;			//Anzahl der schon gelesenen Bits (oben global)
	
	uint16_t sv_r = sn_getLightSenor(S_LS_RR);
	
	//Steigende Flanke
	if(!lastValue && sv_r > I_THRESHOLD){
		lastValue = true;
		maxInBitField = 0;
	}
	//Fallende Flanke
	else if(lastValue && sv_r < I_THRESHOLD){
		lastValue = false;
		
		//Ignorieren der ersten Linie
		if(countBitsRead == 100){
			countBitsRead = 0;
			return;
		}
			
		i_packetAddress[countBitsRead / 3] <<= 1;
		if(maxInBitField > I_THRESHOLD){
			i_packetAddress[countBitsRead / 3] |= 1;
		}
		
		Serial.print(i_packetAddress[0]);
		Serial.print(", ");
		Serial.print(i_packetAddress[1]);
		Serial.print(", ");
		Serial.println(i_packetAddress[2]);
		
		countBitsRead++;
	}
	
	uint16_t sv_l = sn_getLightSenor(S_LS_LL);
	if(sv_l > maxInBitField){
		maxInBitField = sv_l;
	}
	
	db_setRgbLed(0, lastValue, 0);
	
	/*
	if (sn_getLightSenor(S_LS_RR) > I_THRESHOLD){
		db_setRgbLed(1, 0, 0);
	}
	else{
		db_setRgbLed(0, 0, 0);
	}
	*/
	
	//warten bis x steps und dann bit auf der anderen seite lesen
	
	
	
	//Linienfolgen
	mc_compensate();
	
	//Abbruchbedingung
	if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){
		if(sn_getLightSenor(S_LS_RR) > I_THRESHOLD){
			//i_setWorkVariable(mc_getSteps(MC_RIGHT_MOTOR));	
			//i_wait = true;
			mc_stopSneak(MC_LEFT_MOTOR);
			mc_stopSneak(MC_RIGHT_MOTOR);
			mc_resetCompensation();
			i_nextState();
		} 			
	}
}

void i_turnLeft(){
	if(i_initialiser){
		i_initialiser = false;
		mc_move(MC_RIGHT_MOTOR, I_TURN_STEPS_OUTER);
		//mc_setSneak(MC_LEFT_MOTOR, true);
		mc_move(MC_LEFT_MOTOR, -I_TURN_STEPS_INNER);	
		//mc_setSneak(MC_RIGHT_MOTOR, true);
	}
	//if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){
		//if(sn_getLightSenor(S_LS_LM) > I_THRESHOLD){
		if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_STOP ){
			//mc_stopSneak(MC_LEFT_MOTOR);
			//mc_stopSneak(MC_RIGHT_MOTOR);
			i_nextState();
		}
	//}
}

void i_turnRight(){
	if(i_initialiser){
		i_initialiser = false;
		mc_move(MC_LEFT_MOTOR, I_TURN_STEPS_OUTER);
		//mc_setSneak(MC_LEFT_MOTOR, true);
		mc_move(MC_RIGHT_MOTOR, -I_TURN_STEPS_INNER);	
		//mc_setSneak(MC_RIGHT_MOTOR, true);
	}
	Serial.print(mc_getSteps(MC_LEFT_MOTOR));
	Serial.print(", ");
	Serial.println(mc_getSteps(MC_RIGHT_MOTOR));
	//if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){
		//if(sn_getLightSenor(S_LS_LM) > I_THRESHOLD){
		if(mc_getMotorState(MC_LEFT_MOTOR) == MC_STOP ){
			//mc_stopSneak(MC_LEFT_MOTOR);
			//mc_stopSneak(MC_RIGHT_MOTOR);
			i_nextState();
		}
	//}
}

void i_driveToIntersection(){
	uint32_t steps = i_stateOrder_l1[i_stateOrder_pos][1];
	
	if(i_initialiser){
		i_initialiser = false;
		mc_move(MC_LEFT_MOTOR, steps);
		mc_setSneak(MC_LEFT_MOTOR, true);
		mc_move(MC_RIGHT_MOTOR, steps);	
		mc_setSneak(MC_RIGHT_MOTOR, true);
	}
	
	// TODO : handeln was passiert wenn der RR sensor schwarz hat aber noch gefahren werden muss
	//			damit kein zucken ins fahren kommt
	/*
	if(i_getWorkVariable() == 0){
		mc_compensate();
	}
	else {
		i_decWorkVariable();
		Serial.print("Not Compensating: ");
		Serial.print(i_getWorkVariable());
		Serial.print(" Sensor: ");
		Serial.println(sn_getLightSenor(S_LS_RR));
	}
	*/
	mc_compensate();
	if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){
		if(sn_getLightSenor(S_LS_RR) > I_THRESHOLD || sn_getLightSenor(S_LS_LL) > I_THRESHOLD){
			//i_setWorkVariable(mc_getSteps(MC_RIGHT_MOTOR));	
			//i_wait = true;
			mc_stopSneak(MC_LEFT_MOTOR);
			mc_stopSneak(MC_RIGHT_MOTOR);
			mc_resetCompensation();
			i_nextState();
		} 			
	}
	
	/*
	if(i_wait && mc_getSteps(MC_RIGHT_MOTOR) - I_STEPS_AFTER_LINE >= i_getWorkVariable()){
		mc_stopSneak(MC_LEFT_MOTOR);
		mc_stopSneak(MC_RIGHT_MOTOR);
		i_nextState();
	}
	*/
}

//Methode die das Statesystem zurücksetzen
void i_reset(){
	i_initialiser = true;
	i_counter = 0;
	i_wait = false;
	i_stateOrder_pos = 0;
	i_state = i_stateOrder_l1[i_stateOrder_pos][0];
}

// in den nächsten state wechseln und init var zurücksetzen
void i_nextState(){
	i_initialiser = true;
	i_counter = 0;
	i_wait = false;
	i_stateOrder_pos ++;
	i_state = i_stateOrder_l1[i_stateOrder_pos][0];
}

// Methoden um mit der Work variable zu arbeiten damit man den code leichter lesen kann
/*
void i_setWorkVariable(int32_t val){
	i_stateOrder_l1[i_stateOrder_pos][2] = val;
}

uint32_t i_getWorkVariable(){
	return i_stateOrder_l1[i_stateOrder_pos][2];
}

void i_decWorkVariable(){
	i_stateOrder_l1[i_stateOrder_pos][2] --;
}

void i_incWorkVariable(){
	i_stateOrder_l1[i_stateOrder_pos][2] ++;
}
*/
