/*
	Light-Sensorarray
	Aufbau:
	A           B
		  
         C D	   
	
*/
//TODO: set to right pins
#define PIN_L_SENS_FL A0
#define PIN_L_SENS_FR A1
#define PIN_L_SENS_BL A2
#define PIN_L_SENS_BR A3

//For debugging
#define COMP_MULTIPL 10

int sens_fl_white;
int sens_fr_white;
int sens_fl_black;
int sens_fr_black;

int steps_Left = 5000;


void setup(){
	
	pinMode(PIN_L_SENS_FL, INPUT);
	pinMode(PIN_L_SENS_FR, INPUT);
	pinMode(PIN_L_SENS_BL, INPUT);
	pinMode(PIN_L_SENS_BR, INPUT);
	
	calibrateLightSensors();			//calibrate front (lineFollowing) sensors
	delay(5000);
}

void loop(){
	followLineUntilIntersec(stepsLeft);		//follow Line for STEPS steps
}

void calibrateLightSensors(){
	//Read all black
	sens_fl_black = analogRead(PIN_L_SENS_FL);
	sens_fr_black = analogRead(PIN_L_SENS_FR);
	
	//delay for repositioning
	blink();
	delay(3000);
	blink();
	
	sens_fl_white = analogRead(PIN_L_SENS_FL);
	sens_fr_white = analogRead(PIN_L_SENS_FR);
	
}

void blink(){
	digitalWrite(PIN_LED, HIGH);
	delay(100);
	digitalWrite(PIN_LED, LOW);
	
	delay(500);
	
	digitalWrite(PIN_LED, HIGH);
	delay(100);
	digitalWrite(PIN_LED, LOW);
}


int calculateOffset(){
	//Correct input values 
	
}

void updateCompensation(){
	
}


void followLineUntilIntersec(int stepsLeft){
	int offset = calculateOffset();
	updateCompensation(offset);
	
}

