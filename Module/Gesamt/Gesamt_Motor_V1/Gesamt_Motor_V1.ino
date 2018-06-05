#include <Wire.h>
#define I2C_ANZAHLSW 18

//8-Bit Array der, über I2C ankommenden Werte
uint8_t i2c_SensorWerte[I2C_ANZAHLSW];
//Pointer auf start des 16-Bit arrays der übertragenen Sensorwerte
uint16_t *i2c_16BitArray;
//Zähler um, über I2C ankommende 8-Bit Werte, an der richtigen STelle im Array i2c_SensorWerte zu speichern
uint8_t i2c_counter_sensor=0;


void setup() {
  Serial.begin(115200);
  
  //motor_init();
  i2c_init();
  sei();
}

void loop() {
  i2c_print();
  delay(100);
}
