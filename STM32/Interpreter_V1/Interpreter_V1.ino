/*
Allgemein:
	- Überlegen: nicht state-system sondern kontinuierlich?
	- Code nochmal durchschauen und Version für Level 1 fertig stellen
		- Debugs entfernen
		- Interpreter mit Methoden übersichtlicher machen
		- Testen ob auch wirklich alles geht
	- Buttons um Level 1 oder 2 zu starten
	- Motortreiber in ander Modus Testen (Stealthchop)
	- Mit Faktor der Kompensation spielen
	- Zwischen den zwei Level im Interpreter unterscheiden
	- Level 2 einbinden und ramping-Table abhängig vom Level machen

Optimierung:
	- Compensate muss auf Querlininen achten um da nicht in großer Ruckler zu bekommen
	- Mit Zeitlupe filmen warum bei hohen Geschwindigkeiten Fahrfehler auftreten
	- Display zum Zeiten messen und Testen
	- Kurven schneiden und oder bei Kurven die Mittellinie erkennen, geht beim Kurvenschneiden wahrscheinlich nicht

Wenn Mechanik Team soweit ist:
	- Positionen für Ablademechanismus einfügen
	- Zeiten des Ablademechanismusses um eventuell warte Punkte einzubauen
*/

enum{
	MC_LEFT_MOTOR,				//Linker Motor
	MC_RIGHT_MOTOR,				//Rechter Motor
	
	MC_RAMP_UP,					//Motor-State: Beschleunigen
	MC_COAST,					//Motor-State: Geschwindigkeit halten
	MC_RAMP_DOWN,				//Motor-State: Bremsen
	MC_SNEAK,					//Motor-State: Schleichen (um auf Intersection zu warten
	MC_STOP,					//Motor-State: Motor anhalten
	
	S_LS_LL,					//Lichtsenor: Links außen
	S_LS_LM,					//Lichtsenor: Links Mitte
	S_LS_RM,					//Lichtsenor: Rechts Mitte
	S_LS_RR,					//Lichtsenor: Rechts außen
	
	I_LAUNCH,					//Interpreter-State: Erster Status, aus Anfangsfeld losfahren und Bitfelder lesen
	I_WAIT_BUTTON,				//Interpreter-State: Auf Buttoneingabe warten
	I_DRIVE_INTERSECTION,		//Interpreter-State: Fahre bis zur nächsten Kreuzung
	I_TURN_RIGHT,				//Interpreter-State: Rechts um eine Kurve fahren
	I_TURN_LEFT,				//Interpreter-State: Links um eine Kurve fahren
	I_READ_OBSTACLE,			//Interpreter-State: Messen, ob ein Hinderniss im Weg ist
	//I_PREPARE_DELIVER,			//Interpreter-State: Ablage eines Packetes vorbereiten
	//I_DELIVER,					//Interpreter-State: Packet ablegen
	
	I_ROUTE_COMMON,				//Interpreter-Argument: Gemeinsame Route
	I_ROUTE_A,					//Interpreter-Argument: Alternative Route A
	I_ROUTE_B,					//Interpreter-Argument: Alternative Route B
	
	SV_SERVO_SELECTOR,			//Servo: Auswahl des Würfels
	SV_SERVO_HATCH,				//Servo: Öffnen der Klappe
	SV_SERVO_ARM,				//Servo: Bewegen des Auslegerarms
	SV_CUBE_GREEN,				//Würfel: grün
	SV_CUBE_YELLOW,				//Würfel: gelb
	SV_CUBE_RED,				//Würfel: rot
	SV_DIR_RIGHT,				//Abladerichtung: rechts
	SV_DIR_LEFT					//Abladerichtung: links
};

void setup() {
	//Serielle Kommunikation starten
	Serial.begin(115200); 
	
	delay(2000);
	
	//Untergruppen initialisieren
	mc_init();
	sn_init();
	sv_init();
	i_init();
	db_init();
}

void loop() {
	i_loop();
}
