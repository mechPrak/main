
//Mit diesen Variablen können während des Interrupts Werte ausgegeben werden
volatile float debugVariable1 = 0;
volatile float debugVariable2 = 0;

enum{
  MC_LEFT_MOTOR,				//Linker Motor
	MC_RIGHT_MOTOR,				//Rechter Motor
	
	MC_RAMP_UP,					//Motor-State: Beschleunigen
	MC_COAST,					//Motor-State: Geschwindigket halten
	MC_RAMP_DOWN,				//Motor-State: Bremsen
	MC_SNEAK,					//Motor-State: Schleichen (um auf Intersection zu warten
	MC_STOP,					//Motor-State: Motor anhalten
	
	S_LS_LL,					//Lichtsenor: Links außen
	S_LS_LM,					//Lichtsenor: Links Mitte
	S_LS_RM,					//Lichtsenor: Rechts Mitte
	S_LS_RR,					//Lichtsenor: Rechts außen
	
	I_DRIVE_INTERSECTION,		//Interpreter-State: Fahre bis zur nächsten Kreuzung
	I_LAUNCH,					//Interpreter-State: Erster Status, aus Anfangsfeld losfahren, kalibrieren und Bitfelder lesen
	I_TURN_RIGHT,				//Interpreter-State: Rechts um eine Kurve fahren
	I_TURN_LEFT,				//Interpreter-State: Links um eine Kurve fahren
	I_READ_OBSTACLE,			//Interpreter-State: Messen, ob ein Hinderniss im Weg ist
	I_PREPARE_DELIVER,			//Interpreter-State: Ablage eines Packetes vorbereiten
	I_DELIVER,					//Interpreter-State: Packet ablegen
	I_WAIT_BUTTON				//Interpreter-State: Auf Button warten

};

void setup() {
	//Serielle Kommunikation starten
	Serial.begin(115200); 
	
	delay(1000);
	
	//Untergruppen initialisieren
	mc_init();
	sn_init();
	i_init();
	db_init();
}

void loop() {
	i_loop();
	
	//sn_debug();
}
