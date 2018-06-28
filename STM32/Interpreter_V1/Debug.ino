#define PIN_RGB_LED_RED PB12										//Pin für RGB-Led: rot
#define PIN_RGB_LED_GREEN PB13										//Pin für RGB-Led: grün
#define PIN_RGB_LED_BLUE PB14										//Pin für RGB-Led: blau
#define PIN_BUILTIN_LED PC13										//Pin für eingebaute LED

void db_init(){														//Initialisieren der Debug einheiten
	pinMode(PIN_BUILTIN_LED,OUTPUT);
	
	pinMode(PIN_RGB_LED_RED,OUTPUT);
	pinMode(PIN_RGB_LED_GREEN,OUTPUT);
	pinMode(PIN_RGB_LED_BLUE,OUTPUT);
}

void db_setRgbLed(uint8_t red, uint8_t green, uint8_t blue){		//Setzt die RGB-Led auf die gewählte Farbe
	digitalWrite(PIN_RGB_LED_RED, red);
	digitalWrite(PIN_RGB_LED_GREEN, green);
	digitalWrite(PIN_RGB_LED_BLUE, blue);
}

void db_setBuiltinLed(uint8_t value){								//Setzt die Eingebaute Led auf den gewählten Status
	digitalWrite(PIN_BUILTIN_LED, value);
}