String inString = "";
int s = 50;

void setup() {
  Serial.begin(115200);
  pinMode(PB10, OUTPUT);
}

void loop() {
  digitalWrite(PB10, HIGH);
  digitalWrite(PB10, LOW);
  delayMicroseconds(s);

  /*while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
    if (inChar == '\n') {
      Serial.print("Speed:");
      s = inString.toInt();
      Serial.println(s);
      inString = "";
    }
  }*/
}
