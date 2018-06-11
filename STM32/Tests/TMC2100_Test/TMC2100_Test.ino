#define STEP_PIN PC13

int d = 80;

void setup() {
  pinMode(STEP_PIN,   OUTPUT);
}

void loop() {
  digitalWrite(STEP_PIN, HIGH);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(d);
}
