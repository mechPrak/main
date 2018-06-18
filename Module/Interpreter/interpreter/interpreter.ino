/*
   1          2

       3 4       
MA               MB
*/
#define I2CBUFFERWRITE 9

#define sensor1 sensorwerte[3]
#define sensor2 sensorwerte[0]
#define sensor3 sensorwerte[2]
#define sensor4 sensorwerte[1]

volatile uint16_t i2c_bufferW[I2CBUFFERWRITE];

static volatile uint16_t sensorwerte[6];
//magic numbers die getestet werden müssen:
uint16_t cruise_speed_lev1 = 300;
uint16_t turn_speed_lev1 = 100;
uint16_t schwarz_schwellenwert = 300;
uint16_t drehzeit_ms = 2000;
uint16_t bitfeld_zeit = 300;
uint16_t kreuzung_zeit = 200;
//uint16_t steps_bis_kreuzung = 5;

static volatile uint16_t bitwerte[3];

void motorA_speed(uint16_t speed_A){
  
  i2c_bufferW[0] = speed_A;
  
  
}

void motorB_speed(uint16_t speed_B){
  
  i2c_bufferW[1] = speed_B;
  
  
}

void speed_direction(uint16_t speed_dir){

  i2c_bufferW[2] = speed_dir;
  
}

void fahre_speed_time(uint16_t v_mA, uint16_t v_mB, uint16_t speed_dir, uint16_t ms){
  
  motorA_speed(v_mA);
  motorB_speed(v_mB);
  speed_direction(speed_dir);
  pushValues();
  
  delay(ms);
  
  motorA_speed(0);
  motorB_speed(0);
  speed_direction(0b00);
  pushValues();
}

/*void fahre_steps(uint16_t steps){
  for(uint16_t i = 0; i < steps; i++){
    PORTD |= (1<<STEP_A);
    PORTB |= (1<<STEP_B);
    PORTD &= ~(1<<STEP_A);
    PORTB &= ~(1<<STEP_B);
  }
}*/

void fahre(){
  speed_direction(0b00);
  if (sensor3 < sensor4){
      //Serial.println("hallo");
      motorA_speed(cruise_speed_lev1 + 100);
      motorB_speed(cruise_speed_lev1);
      pushValues();
      
    }else if(sensor3 > sensor4){
      //Serial.println("welt");
      motorA_speed(cruise_speed_lev1);
      motorB_speed(cruise_speed_lev1 + 100);
      pushValues();
    
    }
  
}

void fahre_bis_bitfeld(){
  motorA_speed(cruise_speed_lev1);
  motorB_speed(cruise_speed_lev1);
  pushValues();
  
  while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){
    delay(100);
    fahre();
    
  }
  motorA_speed(0);
  motorB_speed(0);
  pushValues();
  
}

void lese_bitfeld(){

  uint16_t bitwerte_1[3];
  uint16_t bitwerte_2[3];
  for(uint8_t i = 0; i < 3; i++){
    
    if (sensor1 > schwarz_schwellenwert){
    
      bitwerte_1[i] = bitwerte_1[i] + 1;
  
    }else if (sensor2 > schwarz_schwellenwert){
    
      bitwerte_2[i] = bitwerte_2[i] + 1;
  
    }
    fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, bitfeld_zeit);
    fahre_bis_bitfeld();

    if (sensor1 > schwarz_schwellenwert){
    
      bitwerte_1[i] = bitwerte_1[i] + 2;
  
    }else if (sensor2 > schwarz_schwellenwert){
    
      bitwerte_2[i] = bitwerte_2[i] + 2;
  
    }
    fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, bitfeld_zeit);
    fahre_bis_bitfeld();

    if (sensor1 > schwarz_schwellenwert){
    
      bitwerte_1[i] = bitwerte_1[i] + 4;
  
    }else if (sensor2 > schwarz_schwellenwert){
    
      bitwerte_2[i] = bitwerte_2[i] + 4;
  
    }
    fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, bitfeld_zeit);
    fahre_bis_bitfeld();

  }

  if (bitwerte_1[0] < bitwerte_2[0]){
    for(uint8_t i = 0; i < 3; i++){
      bitwerte[i] = bitwerte_1[i];
    }
  }else if (bitwerte_1[0] > bitwerte_2[0]){
    for(uint8_t i = 0; i < 3; i++){
      bitwerte[i] = bitwerte_2[i];
    }
  }

}

void fahre_bis_kreuzung(){
  motorA_speed(cruise_speed_lev1);
  motorB_speed(cruise_speed_lev1);
  pushValues();
  
  while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){
    delay(100);
    fahre();
    
  }
  motorA_speed(0);
  motorB_speed(0);
  pushValues();
  
}

void drehe_rechts(){
  /*
  while(sensor3 < schwarz_schwellenwert || sensor4 < schwarz_schwellenwert){
  
    motorA_speed(turn_speed_lev1);
    motorB_speed(turn_speed_lev1);
  }
  */
  fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, kreuzung_zeit);
  
  motorA_speed(0);
  motorB_speed(0);
  pushValues();
  
  fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b01,  drehzeit_ms);

  while(sensor3 > schwarz_schwellenwert && sensor4 > schwarz_schwellenwert){

    /*if (sensor1 < sensor2){
      
      motorA_speed(turn_speed_lev1 + 1);
      motorB_speed(turn_speed_lev1);
      
    }else if(sensor1 > sensor2){
      
      motorA_speed(turn_speed_lev1);
      motorB_speed(turn_speed_lev1 + 1);
    
    }*/
    fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, kreuzung_zeit);
  }

    motorA_speed(0);
    motorB_speed(0);
    pushValues();
}

void drehe_links(){
  /*
  while(sensor3 < schwarz_schwellenwert || sensor4 < schwarz_schwellenwert){
  
    motorA_speed(turn_speed_lev1);
    motorB_speed(turn_speed_lev1);
  }
  */
  fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, kreuzung_zeit);
  
  motorA_speed(0);
  motorB_speed(0);
  pushValues();
  
fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b10, drehzeit_ms);

  while(sensor3 > schwarz_schwellenwert && sensor4 > schwarz_schwellenwert){

    /*if (sensor1 < sensor2){
      
      motorA_speed(turn_speed_lev1 + 1);
      motorB_speed(turn_speed_lev1);
      
    }else if(sensor1 > sensor2){
      
      motorA_speed(turn_speed_lev1);
      motorB_speed(turn_speed_lev1 + 1);
    
    }*/
    fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, kreuzung_zeit);
  }

    motorA_speed(0);
    motorB_speed(0);
    pushValues();
}


void setup() {
  Serial.begin(9600);
  sc_setup();
  i2c_setup();
  motorA_speed(0);
  motorB_speed(0);
  speed_direction(0b00);
  //pushValues();
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  //fahre_speed_time(300, 300, 0b00, 5000);
  //delay(3000);
  //fahre_speed_time(300, 300, 0b11, 5000);
  //delay(3000);
  fahre_bis_kreuzung();
  delay(2000);
  /*i2c_bufferW[0] = 10000;
  i2c_bufferW[1] = 10000;
  i2c_bufferW[2] = 0b00;
  for(uint8_t i=0; i<3; i++){
    Serial.println(i2c_bufferW[i]);
  }
  pushValues();
  delay(3000);
  i2c_bufferW[0] = 0;
  i2c_bufferW[1] = 0;
  i2c_bufferW[2] = 0b00;
  pushValues();
  delay(3000);
  */

}
