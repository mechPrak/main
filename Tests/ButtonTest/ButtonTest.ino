void setup() {
  Serial.begin(115200);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
}

void loop() {
  Serial.print(digitalRead(2));
  Serial.print(digitalRead(3));
  Serial.print(digitalRead(4));
  Serial.print(digitalRead(5));
  Serial.println(digitalRead(6));
}
