/*
	Interpreter auf State-Machine-Basis, bearbeitet Lvl 1 und 2
*/

//------------------------
//		Defines
//------------------------

//Anzahl an states, die nacheinander aufgerufen werden (in Lvl 1)
#define LVL_1_STATE_CNT = 50;

//------------------------
//		LEVEL 1 / LEVEL 2
//------------------------
bool ip_level_1 = true;

//------------------------
//		STATE MACHINE
//------------------------

enum {
	state_read_startBitField,  state_drive_intersection, state_turn_right,
	state_turn_left, state_measure_obstruction,state_prepare_right, state_prepare_left,
	state_deliver_right, state_deliver_left, state_reverse
}

//aktueller State im Interpreter
int ip_state = state_drive_intersection;

//Sequenz, mit der die States durchlaufen werde (mit Argumenten)
int* ip_state_sequence;

//------------------------
//		PAKETAUSWURF
//------------------------

//Flag: Wenn true wird der Paketauswurf rechts / links vorbereitet
bool ip_delivery_right = false;
bool ip_delivery_left = false;



void setup(){
	//Motor-Setup
	mc_setup();
	//ISR_motor-Setup
	mc_isr_setup();
	
	if(ip_level_1){
		ip_state_sequence = new int[LVL_1_STAT_CNT];
	}
	else{
		//Weg berechnen
		//array-speicherplatz freigeben
		//neuen array-speicherplatz holen und beschreiben
	}
}

void loop(){
	
	//State-Machine-Implementierung: 
	switch(ip_state){
		case: state_drive_intersection:
			break;
		case: state_turn_left:
			break;
		case: state_turn_right:
			break;
		case: state_read_startBitField:
			break;
		case: state_measure_obstruction:
			break;
		case: state_prepare_left:
			break;
		case: state_prepare_right:
			break;
		case: state_deliver_left:
			break;
		case: state_deliver_right:
			break;
	}
	
	if(ip_delivery_left){
		
	}
	
	if(ip_delivery_right){
		
	}
	
}

void ISR_motor(){
	//motor-isr Funktion aufrufen
	mc_isr();
}

/*
	Funktion: drive_intersection
	Argumente:
		stepsLeft: Steps, die noch getätigt werden müssen, bevor abgebremst wird
	
	Verlauf:
		Falls noch nicht geschehen, wird auf maximale Geschwindigkeit beschleunigt.
		Solange stepsLeft > 0: Linenfolgen
		Danach: Wenn nächster State drehen erfordert: abbremsen und auf intersection warten
				Sonst:							  	  auf intersection warten
	
*/
void drive_intersection(int stepsLeft){
	
}

/*
	Funktion: turn_left()
*/
void turn_left(){
	
}

void turn_right(){
	
}

void read_startBitField(){
	
}

void measure_obstruction(){
	
}

void deliver_left(){
	
}

void deliver_right(){
	
}