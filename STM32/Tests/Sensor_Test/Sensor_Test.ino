#define PIN_LS_LL PA2
#define PIN_LS_LM PA3
#define PIN_LS_RM PA4
#define PIN_LS_RR PA5


void setup() {
  pinMode(PIN_LS_LL, INPUT);
  pinMode(PIN_LS_LM, INPUT);
  pinMode(PIN_LS_RM, INPUT);
  pinMode(PIN_LS_RR, INPUT);

  Serial.begin(9600);
  Serial.println("Test2");
}

void loop() {
  Serial.print(analogRead(PIN_LS_LL));
  Serial.print(" ");
  Serial.print(analogRead(PIN_LS_LM));
  Serial.print(" ");
  Serial.print(analogRead(PIN_LS_RM));
  Serial.print(" ");
  Serial.println(analogRead(PIN_LS_RR));
  delay(100);
}

