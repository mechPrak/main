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
	
	I_DRIVE_INTERSECTION,
	I_LAUNCH,
	I_TURN_RIGHT,
	I_TURN_LEFT,
	I_READ_OBSTACLE,
	I_PREPARE_DELIVER,
	I_DELIVER,
	I_WAIT_BUTTON

};

void setup() {
	Serial.begin(115200);  
	delay(1000);
	mc_init();
	s_init();
	i_init();
	
	pinMode(PC13,OUTPUT);
	
	pinMode(PB12,OUTPUT);
	pinMode(PB13,OUTPUT);
	pinMode(PB14,OUTPUT);
	
	digitalWrite(PB12, HIGH);
	digitalWrite(PB13, HIGH);
	digitalWrite(PB14, HIGH);
	
	delay(3000);	
}

void loop() {
	i_loop();
		
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
