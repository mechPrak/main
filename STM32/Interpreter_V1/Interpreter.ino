/*
Ideen zu späteren optimierung:
	- Kurven schneiden
		also einen motor nicht anhalten lassen, problem dabei aber wenn einer langsamer wird beginnt
		der roboter sich schon zu drehen un verlässt so die linie
*/


#define I_THRESHOLD 2000

#define I_TURN_STEPS 1000

uint32_t i_state = I_WAIT_BUTTON;

uint32_t i_state_order_lvl_1[50][3];
uint32_t i_state_order_pos = 0;
uint32_t i_state_initialiser = true;

void i_init(){
	i_state_order_lvl_1[0][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[0][1] = 10000;
	i_state_order_lvl_1[0][2] = 0;
	
	i_state_order_lvl_1[1][0] = I_TURN_RIGHT;
	
	i_state_order_lvl_1[2][0] = I_WAIT_BUTTON;
	
	i_state = i_state_order_lvl_1[0][0];
}

void i_loop(){
	switch(i_state){
		case I_WAIT_BUTTON:
			Serial.println("Wait for button");
			
			delay(3000);
			i_reset();
		
			break;
		case I_DRIVE_INTERSECTION:
			i_driveToIntersection();
			break;
		case I_TURN_RIGHT:
			i_turnRight();
			break;
	}
}

void i_turnRight(){
	// turn right
}

void i_driveToIntersection(){
	uint32_t steps = i_state_order_lvl_1[i_state_order_pos][1];
	
	if(i_state_initialiser){
		i_state_initialiser = false;
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
		Serial.println(s_getLightSenor(S_LS_RR));
	}
	*/
	
	mc_compensate();
	
	if(s_getLightSenor(S_LS_RR) > I_THRESHOLD){
		if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){
			mc_stopSneak(MC_LEFT_MOTOR);
			mc_stopSneak(MC_RIGHT_MOTOR);		
			i_nextState();			
		} 			
	}
}

//Methode die das Statesystem zurücksetzen
void i_reset(){
	i_state_initialiser = true;
	i_state_order_pos = 0;
	i_state = i_state_order_lvl_1[i_state_order_pos][0];
}

// in den nächsten state wechseln und init var zurücksetzen
void i_nextState(){
	i_state_initialiser = true;
	i_state_order_pos ++;
	i_state = i_state_order_lvl_1[i_state_order_pos][0];
}

// Methoden um mit der Work variable zu arbeiten damit man den code leichter lesen kann

void i_setWorkVariable(uint32_t val){
	i_state_order_lvl_1[i_state_order_pos][2] = val;
}

uint32_t i_getWorkVariable(){
	return i_state_order_lvl_1[i_state_order_pos][2];
}

void i_decWorkVariable(){
	i_state_order_lvl_1[i_state_order_pos][2] --;
}

void i_incWorkVariable(){
	i_state_order_lvl_1[i_state_order_pos][2] ++;
}