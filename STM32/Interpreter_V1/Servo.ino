#include <Servo.h>					//Arduino-Standard-Library

#define PIN_SV_SELECTOR PB9
#define PIN_SV_HATCH PB8
#define PIN_SV_ARM PB7

Servo sv_servoSelector;				//Servo zur Auswahl des Abzulegenden Würfels
Servo sv_servoHatch;				//Servo zum Öffnen des Schachts über dem Arm
Servo sv_servoArm;					//Servo zum Bewegen des Arms

// servo init
void sv_init(){
	sv_servoSelector.attach(PIN_SV_SELECTOR);
	sv_servoHatch.attach(PIN_SV_HATCH);
	sv_servoArm.attach(PIN_SV_ARM);
}

//servo loop
void sv_loop(){
		
}

void sv_prepare(uint8_t direction, uint8_t color){
	//selector auf farbe setzen
	//warten bis farbe an richtiger pos
	//hatch öffnen
	//warten bis würfel gefallen
	//arm absetzten
}

void sv_drop(){
	//arm hochheben 
	//prepare next
}

void sv_setPos(uint8_t servo, uint8_t pos){
	switch(servo){
		case SV_SERVO_SELECTOR:
			sv_servoSelector.write(pos);
			break;
		case SV_SERVO_HATCH:
			sv_servoHatch.write(pos);
			break;
		case SV_SERVO_ARM:
			sv_servoArm.write(pos);
			break;
	}
}
