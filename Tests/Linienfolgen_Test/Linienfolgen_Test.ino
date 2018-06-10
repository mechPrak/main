uint16_t sensorwerte[11];
enum {ACCEL, DECEL, CONST, CW, CCW};

void setup() {
  motor_init();
  sc_setup();

  motorA_acceleration(1000000);
  motorB_acceleration(1000000);

  motorA_direction(CCW);
  motorB_direction(CCW);

  Serial.begin(115200);
  sei();
}

uint16_t s_1 = 500;
uint16_t s_2 = 501;

void loop() {
  if (sensorwerte[1] > sensorwerte[5]) {
    motorA_speed(s_2);
    motorB_speed(s_1);
  } else {
    motorA_speed(s_1);
    motorB_speed(s_2);
  }
/*
  Serial.print(sensorwerte[1]);
  Serial.print(" ");
  Serial.print(sensorwerte[5]);
  Serial.print(" ");
  Serial.println();*/
  sc_refreshValues();
  delay(10);

}
