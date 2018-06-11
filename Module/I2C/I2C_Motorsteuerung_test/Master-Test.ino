/*
Testcode, der die Motoren an und kurz dararuf wieder ausschaltet. Hat einige Probleme:
  -Die Geschwindigkeit wird nicht richtig übergeben (vermutlich overflow)
  -Zwischen den pushValues() sind (offenbar?) delays() nötig.

*/


//Test der Zahlen
#define MmotorA_direction 1
#define MmotorA_speed 2
#define MmotorA_acceleration 3
#define MmotorA_microStep 4
#define MmotorA_enable 5
#define MmotorA_disable 6

#define MmotorB_direction 11
#define MmotorB_speed 12
#define MmotorB_acceleration 13
#define MmotorB_microStep 14
#define MmotorB_enable 15
#define MmotorB_disable 16

//***Ende Test

//Addresse des Slaves von mir nicht genutzt
#define ADDR 0x08
//Anzahl der zu sendenden 16-Bit Werte
#define I2CBUFFERWRITE 9
//Anzahl der zu empfangenden 16-Bit Werte
#define I2CBUFFERREAD 9

#define LED 13


//i2c_counter ist für den allgemeinen Ablauf der I2C Übertragung
volatile uint8_t i2c_counter=0;
//16-Bit Array für zu sende Werte
volatile uint16_t i2c_bufferW[I2CBUFFERWRITE];
//16-Bit Array für zu empfangende Werte
volatile uint16_t i2c_bufferR[I2CBUFFERREAD];
//Pointer auf begin des 8-Bit Array, welches per I2C versendet wird
volatile uint8_t *i2c_8BitArray; 

void pushValues(){
  //! Aus Schmalex' loop rauskopierte, sollte Datenübertragung einleiten
  //Start der I2C Übertragung
    TWCR = 0b11100101;
    i2c_counter++;
  
  //16-Bit Sensorwerte in, über I2C zu sendendes 8-bit Array speichern
    i2c_8BitArray = (uint8_t *) i2c_bufferW;
}

//Start der ISR jedes mal wenn TWINT gesetzt
ISR(TWI_vect){
//Interrups pausieren
cli();
//Wenn Counter=1, Slave-Adresse senden um zu schreiben (muss eigentlich nicht abgefragt werden) 
  if(i2c_counter == 1){
    //Slave adresse = 8
    TWDR = 0b00010000;        //write mit lsb = 0     read mit lsb = 1
    //setze counter auf 2
    i2c_counter++;
    TWCR = 0b11000101;
  }
  //Wenn Adresse versendet, sende 18 mal 8-Bit Werte an Slave
  else if((1 < i2c_counter) && (i2c_counter < (2*I2CBUFFERWRITE)+2)){
    //Speichere zu sendendes Byte in TWDR
    TWDR = i2c_8BitArray[i2c_counter-2];
 //   Serial.println(TWDR);
    //Zähle counter bis auf 19 (1 mal adresse und 18 mal für die 9 mal 16 byte datenpackete)
    i2c_counter++;
    TWCR = 0b11000101;
  }
  // nach Empfangen der letzten Daten mit repeated start wieder zurück zum Anfang und counter nullen
  else {
    TWCR = 0b11010101;
   // i2c_8BitArray[i2c_counter - ((2*I2CBUFFERWRITE)+5)] = TWDR;
    i2c_counter = 0;

    //Stoppbedingung

    
  }
//Interrupts Aktivieren
sei();
}

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  
  Serial.begin(115200);
//TWI Status Register auf 0 setzen
  TWSR = 0;
  
// Set bit rate
  TWBR = ((F_CPU / 100000) - 16) / 2;  
 
// (TWINT TWEA TWSTA TWSTO TWWC TWEN LEER! TWIE) als standard am Anfang 
  TWCR = 0b11000101;

//hier das array mit werten von Sensoren füttern!

    i2c_bufferW[0] = 0x0101 ;
    i2c_bufferW[1] = 0x0202 ;
    i2c_bufferW[2] = 0x0303 ;
    i2c_bufferW[3] = 0x0404 ;
    i2c_bufferW[4] = 0x0505 ;
    i2c_bufferW[5] = 0x0606 ;
    i2c_bufferW[6] = 0x0707 ;
    i2c_bufferW[7] = 0x0808 ;
    i2c_bufferW[8] = 0xFFFF ;
  
//Interrupts anschalten
   sei();
}

void loop() {
  i2c_bufferW[0]=MmotorA_direction;
  i2c_bufferW[1]=3; //Sollte CW sein -> Schönere Lösung suchen!
  pushValues();
  delay(100);
  i2c_bufferW[0]=MmotorB_direction;
  i2c_bufferW[1]=3;
  pushValues();
  //for(volatile uint32_t i=0; i<1500000; i++){}
  delay(100);
  i2c_bufferW[0]=MmotorA_speed;
  i2c_bufferW[1]=50000;
  pushValues();
  delay(100);
  //delay(100);
  i2c_bufferW[0]=MmotorB_speed;
  i2c_bufferW[1]=50000;
  pushValues();
  delay(2000);
  //delay(100);
  //delay(5000);
  i2c_bufferW[0]=MmotorA_speed;
  i2c_bufferW[1]=0;
  pushValues();
  delay(100);
  i2c_bufferW[0]=MmotorB_speed;
  i2c_bufferW[1]=0;
  pushValues();
  delay(2000);
  /*i2c_bufferW[0]=1;
  digitalWrite(13, HIGH);
  pushValues();
  for(volatile uint32_t i=0; i<150000; i++){}
  i2c_bufferW[0]=0;
  digitalWrite(13, LOW);
  pushValues();
  for(volatile uint32_t i=0; i<150000; i++){}*/
}



