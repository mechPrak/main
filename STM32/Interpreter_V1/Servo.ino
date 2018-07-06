#include <Servo.h>											//Arduino-Standard-Library (wurde für den STM32 geportet)

#define PIN_SV_SELECTOR PB9									//Pin für Servo: Auswahl des Servos 
#define PIN_SV_HATCH PB8									//Pin für Servo: Öffnen der Klappe
#define PIN_SV_ARM PB7										//Pin für Servo: Bewegen des Auslegerarms
#define PIN_SV_UNDEFINED PB6								//Pin für noch nicht definierten Servo


//kommen in interpreter auhc vor
#define ARM_LEFT 164
#define ARM_MID 87
#define ARM_RIGHT 16

#define HATCH_OPEN 85
#define HATCH_CLOSED 120

#define SELECTOR_GREEN 22
#define SELECTOR_YELLOW 85
#define SELECTOR_RED 167

Servo sv_servoSelector;										//Servo zur Auswahl des Abzulegenden Würfels : 7° 	bis 166°
Servo sv_servoHatch;										//Servo zum Öffnen des Schachts über dem Arm : 80° 	bis 115°
Servo sv_servoArm;											//Servo zum Bewegen des Arms 				 : 25°  bis 108°

HardwareTimer servo_timer(3);

uint8_t sv_centerArmFlag = false;
uint8_t sv_loadArmFlag = false;


volatile uint32_t sv_isrCounter = 0;
volatile uint32_t sv_centerCounter = 0;
volatile uint32_t sv_loadArmCounter = 0;

volatile uint32_t sv_color = 0;

void sv_init(){												//Initialisierung der Servos
	sv_servoSelector.attach(PIN_SV_SELECTOR);
	sv_servoHatch.attach(PIN_SV_HATCH);
	sv_servoArm.attach(PIN_SV_ARM);
		
	
	//Timer initialisieren auf 1kHz
	servo_timer.pause();
	servo_timer.setPeriod(1000);
	servo_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
	servo_timer.setCompare(TIMER_CH1, 1);
	servo_timer.attachCompare1Interrupt(sv_ISR);
	servo_timer.refresh();
	servo_timer.resume();
}

void sv_initServos(){
	sv_setPos(SV_SERVO_HATCH, HATCH_CLOSED);
	sv_setPos(SV_SERVO_SELECTOR, SELECTOR_RED);
	sv_centerArm();
}

void sv_ISR(){
	sv_isrCounter++;
	i_delayCounter++;
	if(sv_centerArmFlag){
		sv_centerCounter ++;
		
	}
	if(sv_loadArmFlag){
		sv_loadArmCounter ++;
	}
}

void sv_loop(){
	
	if(sv_centerArmFlag){
		sv_setPos(SV_SERVO_ARM, ARM_MID + 20);
		if(sv_centerCounter > 1000){
			sv_setPos(SV_SERVO_ARM, ARM_MID);
			sv_centerArmFlag = false;
		}
	}
	
	if(sv_loadArmFlag){
		switch(sv_color){
			case 0:
				sv_setPos(SV_SERVO_SELECTOR, SELECTOR_GREEN);
				break;
			case 1:
				sv_setPos(SV_SERVO_SELECTOR, SELECTOR_YELLOW);
				break;
			case 2:
				sv_setPos(SV_SERVO_SELECTOR, SELECTOR_RED);
				break;
		}
		if(sv_loadArmCounter > 500 && sv_loadArmCounter < 550){
			sv_setPos(SV_SERVO_HATCH, HATCH_OPEN);
		}
		
		if(sv_loadArmCounter > 550 && sv_loadArmCounter < 650){
			sv_setPos(SV_SERVO_ARM, ARM_MID - 2);
		}
		if(sv_loadArmCounter > 650 && sv_loadArmCounter < 750){
			sv_setPos(SV_SERVO_ARM, ARM_MID + 4);
		}
		
		if(sv_loadArmCounter > 750 && sv_loadArmCounter < 850){
			sv_setPos(SV_SERVO_ARM, ARM_MID - 2);
		}
		
		if(sv_loadArmCounter > 850 && sv_loadArmCounter < 950){
			sv_setPos(SV_SERVO_ARM, ARM_MID + 4);
		}
		
		if(sv_loadArmCounter > 950 && sv_loadArmCounter < 1050){
			sv_setPos(SV_SERVO_ARM, ARM_MID);
		}
		
		if(sv_loadArmCounter > 2500){
			sv_setPos(SV_SERVO_HATCH, HATCH_CLOSED);
			sv_loadArmFlag = false;
		}
	}
}

void sv_setPos(uint8_t servo, uint8_t pos){					//Setzt die Zielposition des gewählten Servos
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

void sv_centerArm(){
	sv_centerArmFlag = true;
	sv_centerCounter = 0;
	if(i_lastPackage){
		
	}
}

void sv_loadArm(uint8_t nColor){
	sv_loadArmFlag = true;
	sv_loadArmCounter = 0;
	
	sv_color = nColor;
}

void sv_lowerArm(int8_t direction){
	if(direction < 0){
		sv_setPos(SV_SERVO_ARM, ARM_LEFT);
	}
	else {
		sv_setPos(SV_SERVO_ARM, ARM_RIGHT);
	}
}