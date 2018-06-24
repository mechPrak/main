#define PIN_RGB_LED_RED PB12			//Pin für RGB-Led: rot
#define PIN_RGB_LED_GREEN PB13			//Pin für RGB-Led: grün
#define PIN_RGB_LED_BLUE PB14			//Pin für RGB-Led: blau
#define PIN_BUILTIN_LED PC13			//Pin für eingebaute LED


//Pin-Modes setzen
void db_init(){
	pinMode(PIN_BUILTIN_LED,OUTPUT);
	
	pinMode(PIN_RGB_LED_RED,OUTPUT);
	pinMode(PIN_RGB_LED_GREEN,OUTPUT);
	pinMode(PIN_RGB_LED_BLUE,OUTPUT);
}


//RGB LED setzen
void db_setRgbLed(uint8_t red, uint8_t green, uint8_t blue){
	digitalWrite(PIN_RGB_LED_RED, red);
	digitalWrite(PIN_RGB_LED_GREEN, green);
	digitalWrite(PIN_RGB_LED_BLUE, blue);
}

//eingebaute LED setzen
void db_setBuiltinLed(uint8_t value){
	digitalWrite(PIN_BUILTIN_LED, value);
}