#include <Servo.h>											//Arduino-Standard-Library (wurde für den STM32 geportet)

#define PIN_SV_SELECTOR PB9									//Pin für Servo: Auswahl des Servos 
#define PIN_SV_HATCH PB8									//Pin für Servo: Öffnen der Klappe
#define PIN_SV_ARM PB7										//Pin für Servo: Bewegen des Auslegerarms
#define PIN_SV_UNDEFINED PB6								//Pin für noch nicht definierten Servo

Servo sv_servoSelector;										//Servo zur Auswahl des Abzulegenden Würfels : 7° 	bis 166°
Servo sv_servoHatch;										//Servo zum Öffnen des Schachts über dem Arm : 80° 	bis 115°
Servo sv_servoArm;											//Servo zum Bewegen des Arms 				 : 25°  bis 108°

void sv_init(){												//Initialisierung der Servos
	sv_servoSelector.attach(PIN_SV_SELECTOR);
	sv_servoHatch.attach(PIN_SV_HATCH);
	sv_servoArm.attach(PIN_SV_ARM);
}

void sv_prepare(uint8_t direction, uint8_t color){			//Bereitet den nächsten Abwurf vor
	//selector auf farbe setzen
	//warten bis farbe an richtiger pos
	//hatch öffnen
	//warten bis würfel gefallen
	//arm absetzten
}

void sv_drop(){												//Hebt Auslegerarm hoch um den Würfel abzuliefer
	//arm hochheben 
	//prepare next
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
