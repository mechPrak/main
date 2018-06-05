//Addresse des Slaves
#define ADDR 0x08
//Anzahl der zu sendenden 16-Bit Werte
#define I2CBUFFERWRITE 9
//Anzahl der zu empfangenden 16-Bit Werte
#define I2CBUFFERREAD 9

//i2c_counter ist für den allgemeinen Ablauf der I2C Übertragung
volatile uint8_t i2c_counter = 0;
//16-Bit Array für zu sende Werte
volatile uint16_t i2c_bufferW[I2CBUFFERWRITE];
//16-Bit Array für zu empfangende Werte
volatile uint16_t i2c_bufferR[I2CBUFFERREAD];
//Pointer auf begin des 8-Bit Array, welches per I"C versendet wird
volatile uint8_t *i2c_8BitArray;

void i2c_send() {
  //Start der I2C Übertragung
  // set (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)| (1<<TWIE) in TWCR
  TWCR = 0b10100101;
  //Counter auf 1, Startbedingung ausgeführt
  i2c_counter++;
  //16-Bit Sensorwerte in, über I2C zu sendendes 8-bit Array speichern
  i2c_8BitArray = (uint8_t *) i2c_bufferW;
  //Warte 2000 ms
}

void i2c_intit() {
  //TWI Status Register auf 0 setzen
  TWSR = 0;

  // Set bit rate
  TWBR = ((F_CPU / 100000) - 16) / 2;

  // Enable TWI and interrupt
  //set (1 << TWIE) | (1 << TWEN) | (1<<TWINT) in TWCR
  TWCR = 0b10000101;

  //hier das array mit werten von Sensoren füttern!
  for (int i = 0; i < 9; i++) {
    i2c_bufferW[i] = 100 * i;
  }
  //Interrupts anschalten
  sei();
}

//Start der ISR
ISR(TWI_vect) {
  //Interrups pausieren
  cli();
  //Wenn Counter=1, Slave-Adresse senden
  if (i2c_counter == 1) {
    //Slave adresse = 8
    TWDR = 0x10;        //write = 0x00      read = 0x01
    //Sende Inhalt von TWDR über I2C
    //set (1<<TWINT) | (1<<TWEN)| (1<<TWIE) in TWCR
    TWCR = 0b10000101;
    //setze counter auf 2
    i2c_counter++;
  }
  //Wenn Adresse versendet, sende 8-Bit Array an Slave
  else if ((1 < i2c_counter) && (i2c_counter < (2 * I2CBUFFERWRITE) + 2)) {
    //Speichere zu sendendes Byte in TWDR
    TWDR = i2c_8BitArray[i2c_counter - 2];
    //Sende Inhalt von TWDR über I2C
    //set (1<<TWINT) | (1<<TWEN)| (1<<TWIE) in TWCR
    TWCR = 0b10000101;
    //Zähle counter bis auf 20
    i2c_counter++;
  }
  //Kommt Später noch, Code um Daten von Slave zu empfangen
  /*else if(i2c_buffsize_write+1<i2c_counter<i2c_buffsize_write+2){
    i2c_counterW = 0;
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    i2c_counter++;
    }
    else if(i2c_buffsize_write+2<i2c_counter<i2c_buffsize_write+3){
    TWDR = (addr << 1) | 0x01;
    TWCR = (1<<TWINT) | (1<<TWEN);
    i2c_counter++;
    }
    else if(i2c_buffsize_write+3<i2c_counter<i2c_buffsize_write+4){
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    i2c_counter++;
    }
    else if(i2c_buffsize_write+4<i2c_counter<i2c_buffsize_write+4+buffsize_read){
    i2c_bufferR[i2c_counterR] = TWDR;
    i2c_counterR++;
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    i2c_counter++;
    }
    else if(i2c_buffsize_write+4+i2c_buffsize_read<i2c_counter<i2c_buffsize_write+4+i2c_buffsize_read+1){
    i2c_bufferR[i2c_counterR] = TWDR;
    i2c_counterR = 0;
    TWCR = (1<<TWINT) | (1<<TWEN);
    i2c_counter++;
    }
  */
  //Sende Stopbedingung nach Senden des gesammten 8-Bit Arrays
  else {
    //Stoppbedingung
    //set (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE) in TWCR
    TWCR = 0b10010101;
    //Reset Counter
    i2c_counter = 0;
  }
  //Interrupts Aktivieren
  sei();
}
