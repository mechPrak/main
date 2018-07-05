#define PIN_LS_LL PA5								//Pin für Lichtsensor: Links außen
#define PIN_LS_LM PA4								//Pin für Lichtsensor: Links mitten
#define PIN_LS_RM PA3								//Pin für Lichtsensor: Rechts mitte
#define PIN_LS_RR PA2								//Pin für Lichtsensor: Rechts außen

#define PIN_DIST  PA1								//Pin für Enfernungssensor
#define PIN_HALL  PA0								//Pin für Hallsense

#define PIN_BUTTON_1	PB4							//Button für Lvl 1
#define PIN_BUTTON_2	PB5							//Button für Lvl 2

void sn_init() {									//Initialisiern der Sensoren
	//Pin-Modes für Sensoren auf Input setzen
	pinMode(PIN_LS_LL, INPUT);
	pinMode(PIN_LS_LM, INPUT);
	pinMode(PIN_LS_RM, INPUT);
	pinMode(PIN_LS_RR, INPUT);
  
	pinMode(PIN_HALL, INPUT);
	pinMode(PIN_DIST, INPUT);
	
	pinMode(PIN_BUTTON_1, INPUT_PULLUP);
	pinMode(PIN_BUTTON_2, INPUT_PULLUP);
}

uint16_t sn_getLightSenor(uint8_t sensor){			//Gibt den aktuellen Wert des gewählten Helligkeitssensors aus
	switch(sensor){
		case S_LS_LL:
			return analogRead(PIN_LS_LL);
		case S_LS_LM:
			return analogRead(PIN_LS_LM);
		case S_LS_RM:
			return analogRead(PIN_LS_RM);
		case S_LS_RR:
			return analogRead(PIN_LS_RR);
	}
	return 0;
}

uint16_t sn_getDistance(){							//Gibt den aktuellen Wert des Entfernungssensors aus
	return analogRead(PIN_DIST);
}

uint16_t sn_getHallSensor(){
	return analogRead(PIN_HALL);
}

void sn_debug() {									//Gibt alle Sensorwerte per Serial.print zum debuggen aus
	Serial.print("LL: ");
	Serial.print(analogRead(PIN_LS_LL));
	Serial.print("  LM: ");
	Serial.print(analogRead(PIN_LS_LM));
	Serial.print("  RM: ");
	Serial.print(analogRead(PIN_LS_RM));
	Serial.print("  RR: ");
	Serial.print(analogRead(PIN_LS_RR));
	Serial.print("  Dist: ");
	Serial.print(analogRead(PIN_DIST));
	Serial.print("  Hall: ");
	Serial.println(analogRead(PIN_HALL));
	delay(100);
}


uint8_t sn_getButton(uint32_t button){
	if(button == S_BUTTON_1){
		return digitalRead(PIN_BUTTON_1);
	}
	else if (button == S_BUTTON_2){
		return digitalRead(PIN_BUTTON_2);
	}
	return 0;
}