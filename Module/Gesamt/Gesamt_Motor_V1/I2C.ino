// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


void i2c_print() {
  for (int i = 0; i < 9; i++) {
    Serial.print(i2c_16BitArray[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void i2c_init() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  //TESTZWECKE/DEBUGG
  //Serial.begin(115200);           // start serial for output

}



// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while ( Wire.available()) {          // loop through all but the last
    // 8-Bit Array wird mit über i2c ankommenden Werten gefüllt
    i2c_SensorWerte[i2c_counter_sensor] = Wire.read();
    //  testzwecke serial print
    //Serial.println(i2c_SensorWerte[i2c_i_sensor]);
    //counter hochzählen umnächstes über i2c ankommendes Bit in nächste 8-Bit array Position zu speichern
    i2c_counter_sensor++;// print the integer
    //Wenn ein Übertragungszyklus abgeschlossen ist, Counter wieder auf 0 setzen
    if (i2c_counter_sensor == I2C_ANZAHLSW ) {
      i2c_counter_sensor = 0;

    }
    else {}

  }
  //8-bit Array wird in 16-Bit Array umgeschrieben
  i2c_16BitArray = (uint16_t *) i2c_SensorWerte;
}
