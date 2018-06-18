


#define i2c_bufferReadLength 6
#define i2c_bufferWriteLength 6

//besser TWCR = 0b11000101  //mit TWEA = 0 kann man die Verbindung kurz unterbrechen TWINT TWEA TWSTA TWSTO TWWC TWEN leer TWIE
#define TWACK (TWCR = 0b11000101)
//Initial Variables

volatile uint8_t i2c_bufferRead[i2c_bufferReadLength];
volatile uint8_t i2c_bufferWrite[i2c_bufferWriteLength];
volatile int8_t i2c_counter = 0;

void i2c_setup(){
  
  //pullups aktivieren 
  DDRC &= ~(1<< PC4);
  DDRC &= ~(1<< PC5);
  PORTC |= (1<< PC4);
  PORTC |= (1<< PC5);



  
  // Set slave adresse to 8
  TWAR = 0b00010001;

  TWACK;

 for(int i = 0 ; i < 3 ; i++){
  
    i2c_bufferWrite[i]=100*i;
    }
    

  //enable interupts
  sei();
}

ISR(TWI_vect){
  i2cNewData=1;
  //Serial.println(11111);
  //wirft interrupt wenn adresse empfangen
  if(i2c_counter < 1){
    i2c_counter++;
   // Serial.println(222222);
  }
  //wirft Interrupt, wenn Datenbyte empfangen
  else if((0 < i2c_counter) && (i2c_counter < i2c_bufferReadLength+1)){
    // Serial.println(333333);
    i2c_bufferRead[i2c_counter - 1] = TWDR;
   // delay(1);
   // Serial.println(i2c_bufferRead[i2c_counter - 1]);
    //delay(1);
    i2c_counter++;
  }
  //Wirft Interrupt wenn Stoppbedingung empfangen
  else{
    i2c_counter = 0;
  }    
  //8-bit Array wird in 16-Bit Array umgeschrieben
  i2c_16BitArray = (uint16_t *) i2c_bufferRead;
  TWACK; 
}

