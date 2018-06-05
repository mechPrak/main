void setup() {
  pinMode(A3,INPUT);
  Serial.begin(115200);
}
void loop() {
  Serial.println(analogRead(A3));
  delay(10);
}
