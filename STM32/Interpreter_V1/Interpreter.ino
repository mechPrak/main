uint32_t i_state = I_WAIT_BUTTON;

uint32_t state_order_lvl_1[50][2];
uint32_t state_order_pos = 0;

void i_init(){
	state_order_lvl_1[0][0] = I_DRIVE_INTERSECTION;
	state_order_lvl_1[0][1] = 4800;
	
	state_order_lvl_1[1][0] = I_WAIT_BUTTON;
}

void i_loop(){
	switch(i_state){
		case I_WAIT_BUTTON:
			//TODO: IMPLEMENT
			break;
		case I_DRIVE_INTERSECTION:
			i_driveToIntersection();
			break;
	}
}


void i_driveToIntersection(){
	uint32_t steps = state_order_lvl_1[state_order_pos][1];
	
	mc_move(MC_LEFT_MOTOR, steps);
	mc_setSneak(MC_LEFT_MOTOR, true);
	mc_move(MC_RIGHT_MOTOR, steps);	
	mc_setSneak(MC_RIGHT_MOTOR, true);
	
	
}