
#define ADDR 0x08 // slave adresse
#define I2CBUFFERWRITE 9
#define I2CBUFFERREAD 9

volatile uint8_t i2c_counter=0;
volatile uint8_t i2c_bufferW[I2CBUFFERWRITE];
volatile uint8_t i2c_bufferR[I2CBUFFERREAD];
volatile uint8_t i2c_counterW = 0;
volatile uint8_t i2c_counterR = 0;


void setup() {

 // Serial.begin(9600); testing
  //Serial.println(0); testing
  
 
  
  TWSR = 0;
  
  // Set bit rate
  TWBR = ((F_CPU / 100000) - 16) / 2;
  
 // Serial.println(F_CPU); testing
 
  // Enable TWI and interrupt 
  //set (1 << TWIE) | (1 << TWEN) | (1<<TWINT) in TWCR
  TWCR = 0b10000101;
  
 // Serial.println(11); testing

 //hier das array mit werten von Sensoren füttern!
  for( int i = 0; i<9 ; i++ ){ 
    i2c_bufferW[i]=i;

    
 //   Serial.println(12); testing
    
  }
   sei();
}

void loop() {
   // set (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)| (1<<TWIE) in TWCR
   TWCR = 0b10100101;
    i2c_counter++;
 //  Serial.println(2); testing
 delay(20);
 // Serial.println(1); testing

}

ISR(TWI_vect){
cli();

  
  
  if(i2c_counter == 1){
    TWDR = 0x10;        //write = 0x00      read = 0x01
    //set (1<<TWINT) | (1<<TWEN)| (1<<TWIE) in TWCR
    TWCR = 0b10000101;
    i2c_counter++;
  //  Serial.println(3); testing
  }
  else if((1 < i2c_counter) && (i2c_counter < I2CBUFFERWRITE+2)){
    TWDR = i2c_bufferW[i2c_counterW];
    i2c_counterW++;
    //set (1<<TWINT) | (1<<TWEN)| (1<<TWIE) in TWCR
    TWCR = 0b10000101;
    i2c_counter++;
  //  Serial.println(5);
  }
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
*/  else{
    i2c_counterW = 0;     //zeile löschen wenn slave auch zurücksendet
    //set (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE) in TWCR
    TWCR = 0b10010101;
    i2c_counter =0;
   // Serial.println(6); testing
  }
  //Serial.println(8); testing
sei();
}

