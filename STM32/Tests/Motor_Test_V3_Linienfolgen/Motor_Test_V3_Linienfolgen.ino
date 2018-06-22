volatile float debugVariable1 = 0;
volatile float debugVariable2 = 0;

enum{
    MC_LEFT_MOTOR,
	MC_RIGHT_MOTOR,
	
	MC_RAMP_UP,
	MC_COAST,
	MC_RAMP_DOWN,
	MC_SNEAK,
	MC_STOP,
	
	S_LS_LL,
	S_LS_LM,
	S_LS_RM,
	S_LS_RR,
};

void setup() {
	Serial.begin(115200);  
	delay(1000);
	mc_init();
	s_init();
}

void loop() {
		
		
		
	/*
	uint32_t move = true;
	while(move){
		
		mc_compensate();		
		
		if(s_getLightSenor(S_LS_RR) > 2000){
			mc_stopSneak(MC_LEFT_MOTOR);
			mc_stopSneak(MC_RIGHT_MOTOR);
			move = false;
		}
		
		delay(10);
	}
	
	delay(3000);
	*/
}

void mc_compensate(){
	float ratio = (float)s_getLightSenor(S_LS_RM) / (float)s_getLightSenor(S_LS_LM);
	ratio = log(ratio);
	ratio = ratio / 10;
	mc_setCompensation(MC_RIGHT_MOTOR, ratio);
	mc_setCompensation(MC_LEFT_MOTOR, -ratio);
}

void i_driveToIntersection(uint32_t steps){
	mc_move(MC_LEFT_MOTOR, steps);	
	mc_setSneak(MC_LEFT_MOTOR, true);
	mc_move(MC_RIGHT_MOTOR, steps);	
	mc_setSneak(MC_RIGHT_MOTOR, true);
	
	
}