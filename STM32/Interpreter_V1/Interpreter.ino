#define I_MM_TO_STEPS_FAKTOR (50/3)						//mm in steps -> mm * 50/3

#define I_LS_THRESHOLD 2000								//Wert des Helligkeitsensors ab dem Schwarz erkannt werden soll
#define I_D_THRESHOLD 700								//Wert des Entfernungssensors ab dem ein Obstacle erkannt wird

#define I_TURN_STEPS_OUTER 2550							//Steps die der äußere Motor fährt
#define I_TURN_STEPS_INNER 700							//Steps die der innere Motor fährt
#define I_TURNSTEPS 1300								//Steps die nach einer Kurve abgezogen werden
#define I_SNEAKSTEPS 100

#define I_LAUNCH_STEPS 4500								//Steps für die Anfangsphase mit Bitfeldern
#define I_BIT_READ_STEPDELAY 150						//Steps die nach der steigenden Flanke noch gewartet werden bevor Bit gelesen wird

#define I_STEPS_AFTER_INTERSECTION 150					//Steps, die nach einer Intersection gewartet werden, bevor bei der Paketausgabe auf den Lichtsensor getriggert wird

uint8_t i_obstaclePositions[3] = {false, false, false};	//Positionen der Hindernisse für Level 2
uint8_t i_currentRoute = I_ROUTE_A;						//Route die gefahren werden muss um dem nächsten Hinderniss auszuweichen
uint8_t i_packetAddress[3] = {0, 0, 0};					//gelesene Adressen: 0 -> grün, 1-> gelb, 2 -> rot

uint8_t i_currentLevel = 0;								//Aktuelles Level
uint8_t i_stateOrder_pos = 0;							//Position im Statearray des Aktuellen Levels
uint8_t i_state = I_WAIT_BUTTON;						//Aktueller state (initial: auf Knopfdruck warten)
uint8_t i_initialiser = true;							//Boolean um States zu initialisieren

uint8_t i_deliverEnable = false;
uint32_t i_deliverInSteps = 0;



//Reihenfolge und Argument der States der Level 1 und 2
//Argument 0: State
//Argument 1: bei I_DRIVE_INTERSECTION: Anzahl der zu fahrenden Steps. Sonst: undefiniert
//Argument 2: Routenzuordnung (COMMON gilt für beide Routen, A ist die jeweils äußere, B die innere)
//Argument 3: Abladefeld und Richtung (Positv heißt rechts ist das Referenzbitfeld)
int32_t i_stateOrder_l1[][4] = {
	
	{I_LAUNCH,				0,							I_ROUTE_COMMON,		0},	
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

	{I_DRIVE_INTERSECTION, 	315 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		-5},
	{I_TURN_RIGHT,			0,							I_ROUTE_COMMON,		0},
	{I_DRIVE_INTERSECTION, 	350 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		1},
	
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
	
	{I_DRIVE_INTERSECTION, 	335 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		2},
	{I_TURN_RIGHT,			0,							I_ROUTE_COMMON,		0},
	{I_DRIVE_INTERSECTION, 	205 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		3},
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
		
	{I_DRIVE_INTERSECTION, 	245 * I_MM_TO_STEPS_FAKTOR,	I_ROUTE_COMMON,		-4},
	{I_WAIT_BUTTON,			0,							I_ROUTE_COMMON,		0}
};


void i_init(){//Initialisierung des Interpreters
	//TODO: setzen des Aktuellen Levels durch waitButton
	i_currentLevel = 1;	
	//aktueller State wird auf den ersten State in der Tabelle gesetzt
	i_state = i_stateOrder_l1[0][0];
}


void i_loop(){//Hauptloop des interpreters
	
	
	//Impementierung des State-Systems
	switch(i_state){
		case I_LAUNCH:
			i_launch();
			break;
			
		case I_WAIT_BUTTON:			
			//TODO: IMPLEMENT	
			delay(5000);
			i_reset();		
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
	}
}


void i_deliver(uint8_t number){//Temporäre Ablademethode

	sv_setPos(SV_SERVO_ARM, 100);								//Zu debug-Zwecken den Servo bewegen
	
	switch(number){												//Entsprechende Farbe durch RGB-LED leuchten lassen
		case 1:
			db_setRgbLed(1,0,0);
			break;
		case 2:
			db_setRgbLed(0,1,0);
			break;
		case 3:
			db_setRgbLed(0,0,1);
			break;
		case 4:
			db_setRgbLed(1,1,0);
			break;
		case 5:
			db_setRgbLed(0,1,1);
			break;
	}
}


void i_launch(){//Erster State um die Bifelder am Anfang auszulesen
	//Initialiser wird einmal am Anfang des States aufgerufen
	if(i_initialiser){
		i_initialiser = false;							//Initialiser wird damit das nächste Mal nicht mehr aufgerufen
		mc_move(MC_LEFT_MOTOR, I_LAUNCH_STEPS);			//Motoren bis zur Kreuzung fahren lassen
		mc_setSneak(MC_LEFT_MOTOR, true);
		mc_move(MC_RIGHT_MOTOR, I_LAUNCH_STEPS);	
		mc_setSneak(MC_RIGHT_MOTOR, true);
	}
	
	//Bifelder lesen
	static uint8_t lastValue = 0;								//letzter gemessener Wert des rechten Bitfelds
	static uint32_t maxInBitField = 0;							//maximal gemessener Wert in einem Bit
	static uint8_t countBitsRead = 100;							//Anzahl der schon gelesenen Bits (auf Hundert um die erste Linie zu ignorieren)
	
	uint16_t sv_r = sn_getLightSenor(S_LS_RR);					//Auslesen des rechten Sensors zum erkennen der Referenz-Bitlfelder
	uint16_t sv_l = sn_getLightSenor(S_LS_LL);					//Auslesen des linken Sensors zum erkennen der Daten-Bitlfelder
	
	if(sv_l > maxInBitField){									//Maximalen Wert von LL merken
		maxInBitField = sv_l;									
	}	
	
	if(!lastValue && sv_r > I_LS_THRESHOLD){					//Steigende Flanke an RR
		lastValue = true;
		maxInBitField = 0;										//max-Wert zurücksetzen
	}
	
	else if(lastValue && sv_r < I_LS_THRESHOLD){				//Fallende Flanke an RR
		lastValue = false;
		
		if(countBitsRead == 100){								//Ignorieren der ersten Linie durch Workaround
			countBitsRead = 0;
			return;
		}
		
		i_packetAddress[countBitsRead / 3] <<= 1;				//Adresse nach Links shiften
		if(maxInBitField > I_LS_THRESHOLD){						//Wenn wert schwarz war, dann Bit auf 1 setzen
			i_packetAddress[countBitsRead / 3] |= 1;			
		}
		countBitsRead++;
	}
	
	db_setRgbLed(lastValue, lastValue, lastValue);				//Debug: Led auf Wert des Bitfeldes setzen
	
	mc_compensate();											//Linienfolgen
	
	if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){			//Abbruchbedingung
		if(sn_getLightSenor(S_LS_RR) > I_LS_THRESHOLD){
			mc_stopSneak(MC_LEFT_MOTOR);						//Motoren anhalten
			mc_stopSneak(MC_RIGHT_MOTOR);
			mc_resetCompensation();								//Compensation auf 0 setzen
			i_nextState();										//In den nächsten State gehen
		} 			
	}
}


void i_readObstacle(){//Erkennen eines Hindernisses und dementsprechend i_currentRoute und i_obstaclePositions setzen

	if(sn_getDistance() > I_D_THRESHOLD){																	//Wenn ein Hinderniss im Weg ist
		i_obstaclePositions[i_stateOrder_l1[i_stateOrder_pos][1]] = true;									//i_obstaclePositions an dieser Stelle auf false setzen
																											// -> Die Position im Array wird aus dem State ausgelesen
		i_currentRoute = I_ROUTE_B;																			//Route auf B setzen
	}
	else {																									//Wenn kein Hinderniss im Weg ist
		i_obstaclePositions[i_stateOrder_l1[i_stateOrder_pos][1]] = false;									//i_obstaclePositions an dieser Stelle auf false setzen
		i_currentRoute = I_ROUTE_A;																			//Route auf A setzen
	}
	
	i_nextState();																							//Nächsten State aufrufen
}


void i_driveToIntersection(){//Bis zur nächsten Kreuzung fahren
	if(i_initialiser){																						//Initialiser wird einmal am Anfang aufgerufen
		i_initialiser = false;																				//Damit wird der Initialiser das nächste mal nicht mehr aufgeführt
		
		uint32_t steps = i_stateOrder_l1[i_stateOrder_pos][1];												//Steps fürs erste aus der state_order auslesen		
		
		//---------------------------------------------------
		//		ABZIEHEN DER TURNSTEPS (FALLS NOTWENDIG)
		//---------------------------------------------------
		
		uint8_t statesBack = 1;																				//Schritte zurück, bis letzter relevanter State kommt (z.B. alle ROUTE_B überspringen)
		
		//Wenn im vorherigen State Route A ist und jetzige Route aber B oder
		//wenn im vorherigen State Route B ist und jetzige Route aber A 
		//Dann einen Schritt weiter
		//somit geht man bis zu richtigen Route oder Common Route		
		while(i_stateOrder_l1[i_stateOrder_pos - statesBack][2] == I_ROUTE_A && i_currentRoute == I_ROUTE_B || 
		      i_stateOrder_l1[i_stateOrder_pos - statesBack][2] == I_ROUTE_B && i_currentRoute == I_ROUTE_A){
			statesBack++;
		}
		
		//Wenn statesBack stellen zurück ein Turn war 
		if(i_stateOrder_l1[i_stateOrder_pos - statesBack][0] == I_TURN_LEFT || i_stateOrder_l1[i_stateOrder_pos - statesBack][0] == I_TURN_RIGHT){
			steps -= I_TURNSTEPS;
		}	
		
		
		
		//---------------------------------------------------
		//		DRIVE_TO_INTERSECT-Zustände kombinieren
		//---------------------------------------------------

		uint8_t statesForward = 1;																			
		bool stopLooking = true;
		uint8_t numberIntersections = 0;
		
		//Wenn im nächsten State Route A ist und jetzige Route aber B oder
		//wenn im nächsten State Route B ist und jetzige Route aber A 
		//Dann einen Schritt weiter
		//somit geht man bis zu richtigen Route oder Common Route	
		while(stopLooking){
			while(i_stateOrder_l1[i_stateOrder_pos + statesForward][2] == I_ROUTE_A && i_currentRoute == I_ROUTE_B || 
				  i_stateOrder_l1[i_stateOrder_pos + statesForward][2] == I_ROUTE_B && i_currentRoute == I_ROUTE_A){
				statesForward++;
			}
			if(i_stateOrder_l1[i_stateOrder_pos + statesForward][0] == I_DRIVE_INTERSECTION){							//DriveIntersection zusammenzählen
				numberIntersections++;
				Serial.println("combine");
				if(i_stateOrder_l1[i_stateOrder_pos + statesForward][3] != 0){
					Serial.println("check for deliver");
					//TODO: Nur auf true setzen, wenn das jetzige Feld auch in der Abladeliste vorkommt
					if((i_packetAddress[0] == abs(i_stateOrder_l1[i_stateOrder_pos + statesForward][3])) ||
					(i_packetAddress[1] == abs(i_stateOrder_l1[i_stateOrder_pos + statesForward][3])) ||
					(i_packetAddress[2] == abs(i_stateOrder_l1[i_stateOrder_pos + statesForward][3]))){
						i_deliverEnable = true;
						i_deliverInSteps = steps + I_STEPS_AFTER_INTERSECTION;											//Falls einer der Zustände die Ablieferung eines Würfels beinhaltet, behandeln
					}
					
				}
				steps += i_stateOrder_l1[i_stateOrder_pos + statesForward][1];
				statesForward++;
			}
			else{																										//Ansonsten kann die vorrausschauende Suche abgebrochen werden
				
				stopLooking = false;
				i_stateOrder_pos += (statesForward - 1);
			}
		}	
		if(!i_deliverEnable){
			Serial.println("2tes wird aufgerufen");
			if(i_stateOrder_l1[i_stateOrder_pos][3] != 0){
				Serial.println("check for deliver(2)");
				//TODO: Nur auf true setzen, wenn das jetzige Feld auch in der Abladeliste vorkommt
				if((i_packetAddress[0] == abs(i_stateOrder_l1[i_stateOrder_pos][3])) ||
					(i_packetAddress[1] == abs(i_stateOrder_l1[i_stateOrder_pos][3])) ||
					(i_packetAddress[2] == abs(i_stateOrder_l1[i_stateOrder_pos][3]))){
					i_deliverEnable = true;
					
					//DeliverInSteps setzen
					i_deliverInSteps = I_STEPS_AFTER_INTERSECTION;
				}
			}
		}
		steps -= I_SNEAKSTEPS;																							//Sneaksteps abziehen
		
		//Fahren
		mc_move(MC_LEFT_MOTOR, steps);
		mc_setSneak(MC_LEFT_MOTOR, true);
		mc_move(MC_RIGHT_MOTOR, steps);
		mc_setSneak(MC_RIGHT_MOTOR, true);
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
	if(i_deliverEnable){
		
		if(mc_getSteps(MC_RIGHT_MOTOR) > i_deliverInSteps){
			int16_t sensor = -1;
			
			if(i_stateOrder_l1[i_stateOrder_pos][3] > 0){
				sensor = sn_getLightSenor(S_LS_RR);
			}
			else if(i_stateOrder_l1[i_stateOrder_pos][3] < 0){
				sensor = sn_getLightSenor(S_LS_LL);
			}
			
			//Wenn ein Paket ausgeliefert werden soll und ob das Bitfeld schon erreicht ist
			if(sensor != -1 && sensor > I_LS_THRESHOLD){
				//Jetzt ablegen
				i_deliver(abs(i_stateOrder_l1[i_stateOrder_pos][3]));
				Serial.println("Paket wurde abgelegt");
				//Zurücksetzen
				i_deliverEnable = false;
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


void i_turnLeft(){//Nach links abbiegen
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


void i_reset(){//Interpreter zurücksetzen
	i_initialiser = true;																					//Initialiser zurücksetzen
	i_packetAddress[0] = 0;																					//Paketadressen zurücksetzen
	i_packetAddress[1] = 0;
	i_packetAddress[2] = 0;
	i_stateOrder_pos = 0;																					//Statepos zurücketzen
	i_state = i_stateOrder_l1[i_stateOrder_pos][0];															//State wieder auf den Anfangsstate setzen
}


void i_nextState(){//Nächsten State aufrufen
	sv_setPos(SV_SERVO_ARM, 0);																				//FÜR DEBUG: Servo zurückbewegen
	
	i_initialiser = true;																					//Initialiser wieder zurücksetzen
	i_stateOrder_pos ++;																					///State hochzählen
	
	while(i_currentRoute == I_ROUTE_A && i_stateOrder_l1[i_stateOrder_pos][2] == I_ROUTE_B){				//States überspringen, die nicht ausgeführt werden müssen ( B überspringen)
		i_stateOrder_pos++;
	}
	
	while(i_currentRoute == I_ROUTE_B && i_stateOrder_l1[i_stateOrder_pos][2] == I_ROUTE_A){				//States überspringen, die nicht ausgeführt werden müssen ( A überspringen)
		i_stateOrder_pos++;
	}
	
	i_state = i_stateOrder_l1[i_stateOrder_pos][0];															//neuen State setzen
}