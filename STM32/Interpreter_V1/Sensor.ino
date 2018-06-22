#define PIN_LS_LL PA5
#define PIN_LS_LM PA4
#define PIN_LS_RM PA3
#define PIN_LS_RR PA2

void s_init() {
  pinMode(PIN_LS_LL, INPUT);
  pinMode(PIN_LS_LM, INPUT);
  pinMode(PIN_LS_RM, INPUT);
  pinMode(PIN_LS_RR, INPUT);
}

uint32_t s_getLightSenor(int sensor){
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
}

void s_debug() {
  Serial.print(analogRead(PIN_LS_LL));
  Serial.print(" ");
  Serial.print(analogRead(PIN_LS_LM));
  Serial.print(" ");
  Serial.print(analogRead(PIN_LS_RM));
  Serial.print(" ");
  Serial.println(analogRead(PIN_LS_RR));
}

