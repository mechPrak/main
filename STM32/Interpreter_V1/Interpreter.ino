/*
Ideen zu späteren optimierung:
	- Kurven schneiden
		also einen motor nicht anhalten lassen, problem dabei aber wenn einer langsamer wird beginnt
		der roboter sich schon zu drehen un verlässt so die linie
*/


#define I_LS_THRESHOLD 2000

#define I_TURN_STEPS_OUTER 2350
#define I_TURN_STEPS_INNER 700

#define I_STEPS_AFTER_LINE 200

uint32_t i_state = I_WAIT_BUTTON;

uint32_t i_state_order_lvl_1[50][3];
uint32_t i_state_order_pos = 0;
uint32_t i_state_initialiser = true;
uint32_t i_counter = 0;
uint32_t i_wait = false;

void i_init(){
	i_state_order_lvl_1[0][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[0][1] = 15000;	
	i_state_order_lvl_1[1][0] = I_TURN_RIGHT;
	
	i_state_order_lvl_1[2][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[2][1] = 4000;	
	i_state_order_lvl_1[3][0] = I_TURN_RIGHT;
	
	i_state_order_lvl_1[4][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[4][1] = 750;	
	i_state_order_lvl_1[5][0] = I_TURN_LEFT;
	
	i_state_order_lvl_1[6][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[6][1] = 1500;	
	i_state_order_lvl_1[7][0] = I_TURN_RIGHT;
	
	i_state_order_lvl_1[8][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[8][1] = 4000;	
	i_state_order_lvl_1[9][0] = I_TURN_RIGHT;
	
	i_state_order_lvl_1[10][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[10][1] = 1500;	
	i_state_order_lvl_1[11][0] = I_TURN_LEFT;
	
	i_state_order_lvl_1[12][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[12][1] = 750;	
	i_state_order_lvl_1[13][0] = I_TURN_LEFT;
	
	i_state_order_lvl_1[14][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[14][1] = 1500;	
	i_state_order_lvl_1[15][0] = I_TURN_RIGHT;
	
	i_state_order_lvl_1[16][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[16][1] = 4000;
	i_state_order_lvl_1[17][0] = I_TURN_RIGHT;
	
	i_state_order_lvl_1[18][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[18][1] = 4000;
	i_state_order_lvl_1[19][0] = I_TURN_RIGHT;
	
	i_state_order_lvl_1[20][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[20][1] = 4000;
	i_state_order_lvl_1[21][0] = I_TURN_LEFT;
	
	i_state_order_lvl_1[22][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[22][1] = 750;
	i_state_order_lvl_1[23][0] = I_TURN_LEFT;
	
	i_state_order_lvl_1[24][0] = I_DRIVE_INTERSECTION;
	i_state_order_lvl_1[24][1] = 4000;
	
	
	
	i_state_order_lvl_1[25][0] = I_WAIT_BUTTON;
	
	i_state = i_state_order_lvl_1[0][0];
}

void i_loop(){
	switch(i_state){
		case I_WAIT_BUTTON:
			Serial.println("Wait for button");
			
			delay(5000);
			i_reset();
		
			break;
		case I_DRIVE_INTERSECTION:
			Serial.println("Drive to intersection");
			i_driveToIntersection();
			break;
		case I_TURN_RIGHT:
		Serial.println("Turn right");
			i_turnRight();
			break;
		case I_TURN_LEFT:
			Serial.println("Turn right");
			i_turnLeft();
			break;
	}
}

void i_turnLeft(){
	if(i_state_initialiser){
		i_state_initialiser = false;
		mc_move(MC_RIGHT_MOTOR, I_TURN_STEPS_OUTER);
		//mc_setSneak(MC_LEFT_MOTOR, true);
		mc_move(MC_LEFT_MOTOR, -I_TURN_STEPS_INNER);	
		//mc_setSneak(MC_RIGHT_MOTOR, true);
	}
	//if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){
		//if(s_getLightSenor(S_LS_LM) > I_LS_THRESHOLD){
		if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_STOP ){
			//mc_stopSneak(MC_LEFT_MOTOR);
			//mc_stopSneak(MC_RIGHT_MOTOR);
			i_nextState();
		}
	//}
}

void i_turnRight(){
	if(i_state_initialiser){
		i_state_initialiser = false;
		mc_move(MC_LEFT_MOTOR, I_TURN_STEPS_OUTER);
		//mc_setSneak(MC_LEFT_MOTOR, true);
		mc_move(MC_RIGHT_MOTOR, -I_TURN_STEPS_INNER);	
		//mc_setSneak(MC_RIGHT_MOTOR, true);
	}
	//if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){
		//if(s_getLightSenor(S_LS_LM) > I_LS_THRESHOLD){
		if(mc_getMotorState(MC_LEFT_MOTOR) == MC_STOP ){
			//mc_stopSneak(MC_LEFT_MOTOR);
			//mc_stopSneak(MC_RIGHT_MOTOR);
			i_nextState();
		}
	//}
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
	if(mc_getMotorState(MC_RIGHT_MOTOR) == MC_SNEAK){
		if(s_getLightSenor(S_LS_RR) > I_LS_THRESHOLD || s_getLightSenor(S_LS_LL) > I_LS_THRESHOLD){
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
	i_state_initialiser = true;
	i_counter = 0;
	i_wait = false;
	i_state_order_pos = 0;
	i_state = i_state_order_lvl_1[i_state_order_pos][0];
}

// in den nächsten state wechseln und init var zurücksetzen
void i_nextState(){
	i_state_initialiser = true;
	i_counter = 0;
	i_wait = false;
	i_state_order_pos ++;
	i_state = i_state_order_lvl_1[i_state_order_pos][0];
}

// Methoden um mit der Work variable zu arbeiten damit man den code leichter lesen kann

void i_setWorkVariable(int32_t val){
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