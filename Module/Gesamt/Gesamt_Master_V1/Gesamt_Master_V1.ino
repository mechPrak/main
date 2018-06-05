void setup() {
  sc_setup();
  i2c_intit();
  Serial.begin(115200);
  sei();
}

void loop() {
  sc_print();
  i2c_send();
  delay(100);
}
