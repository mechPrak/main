/*
   1          2

       3 4       
MA               MB
*/
#define I2CBUFFERWRITE 9

#define sensor1 sensorwerte[1]
#define sensor2 sensorwerte[0]
#define sensor3 sensorwerte[3]
#define sensor4 sensorwerte[2]
volatile uint16_t i2c_bufferW[I2CBUFFERWRITE];

static volatile uint16_t sensorwerte[6];
//magic numbers die getestet werden müssen:
uint16_t cruise_speed_lev1 = 300;
uint16_t turn_speed_lev1 = 100;
uint16_t schwarz_schwellenwert = 650;
uint16_t drehzeit_ms = 2000;
uint16_t bitfeld_zeit = 2000;
uint16_t kreuzung_zeit = 200;
//uint16_t steps_bis_kreuzung = 5;

static volatile uint16_t bitwerte[3];

void set_motor_speed_direction(uint16_t motor_speed_A, uint16_t motor_speed_B, uint16_t dir){
  i2c_bufferW[0] = motor_speed_A;
  i2c_bufferW[1] = motor_speed_B;
  i2c_bufferW[2] = dir;
  pushValues();
}

void fahre_speed_time(uint16_t v_mA, uint16_t v_mB, uint16_t speed_dir, uint16_t ms){
  
  set_motor_speed_direction(v_mA, v_mB, speed_dir);
  
  delay(ms);
  
  set_motor_speed_direction(0, 0, speed_dir);
  
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

  if (sensor3 < sensor4){
      
      set_motor_speed_direction(cruise_speed_lev1 + 1, cruise_speed_lev1, 0b00);
      
    }else if(sensor3 > sensor4){
      
      set_motor_speed_direction(cruise_speed_lev1, cruise_speed_lev1 + 1, 0b00);
    
    }
  
}

void fahre_bis_bitfeld(){
  set_motor_speed_direction(cruise_speed_lev1, cruise_speed_lev1, 0b00);
  
  while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){
    
    fahre();
    
  }
  set_motor_speed_direction(0, 0, 0b00);
  
}

void lese_bitfeld(){
  
  uint16_t bitwerte_1[3];
  bitwerte_1[0] = 0;
  bitwerte_1[1] = 0;
  bitwerte_1[2] = 0;
  uint16_t bitwerte_2[3];
  bitwerte_2[0] = 0;
  bitwerte_2[1] = 0;
  bitwerte_2[2] = 0;
  fahre_bis_kreuzung();
  for(uint8_t i = 0; i < 3; i++){
    
    if (sensor1 > schwarz_schwellenwert){
    
      bitwerte_1[i] = bitwerte_1[i] + 1;
  
    }else if (sensor2 > schwarz_schwellenwert){
    
      bitwerte_2[i] = bitwerte_2[i] + 1;
  
    }

    while(sensor1 > schwarz_schwellenwert || sensor2 > schwarz_schwellenwert){}
    while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){}

    if (sensor1 > schwarz_schwellenwert){
    
      bitwerte_1[i] = bitwerte_1[i] + 2;
  
    }else if (sensor2 > schwarz_schwellenwert){
    
      bitwerte_2[i] = bitwerte_2[i] + 2;
  
    }
    while(sensor1 > schwarz_schwellenwert || sensor2 > schwarz_schwellenwert){}
    while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){}

    if (sensor1 > schwarz_schwellenwert){
    
      bitwerte_1[i] = bitwerte_1[i] + 4;
  
    }else if (sensor2 > schwarz_schwellenwert){
    
      bitwerte_2[i] = bitwerte_2[i] + 4;
  
    }
    while(sensor1 > schwarz_schwellenwert || sensor2 > schwarz_schwellenwert){}
    while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){}

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
  set_motor_speed_direction(cruise_speed_lev1, cruise_speed_lev1, 0b00);
  
  while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){
    
    fahre();
    
    
  }
  set_motor_speed_direction(0, 0, 0b00);
  
}

void drehe_rechts(){
  /*
  while(sensor3 < schwarz_schwellenwert || sensor4 < schwarz_schwellenwert){
  
    motorA_speed(turn_speed_lev1);
    motorB_speed(turn_speed_lev1);
  }
  */
  fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, kreuzung_zeit);
  
  set_motor_speed_direction(0, 0, 0b00);
  
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

    set_motor_speed_direction(0, 0, 0b00);
}

void drehe_links(){
  /*
  while(sensor3 < schwarz_schwellenwert || sensor4 < schwarz_schwellenwert){
  
    motorA_speed(turn_speed_lev1);
    motorB_speed(turn_speed_lev1);
  }
  */
  fahre_speed_time(turn_speed_lev1, turn_speed_lev1, 0b00, kreuzung_zeit);
  
  set_motor_speed_direction(0, 0, 0b00);
  
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

    set_motor_speed_direction(0, 0, 0b00);
}


void setup() {
  Serial.begin(9600);
  sc_setup();
  i2c_setup();
  set_motor_speed_direction(0, 0, 0b00);
  //pushValues();
}

void loop() {
  //Serial.println("1:");
  /*fahre_speed_time(300, 300, 0b00, 5000);
  delay(3000);
  //Serial.println("2:");
  fahre_speed_time(300, 300, 0b11, 5000);
  delay(3000);*/
  //uint8_t counter = 0;
  //if (counter == 0){
    /*fahre_speed_time(300,300,0b00,2000);
    delay(2000);
    fahre_speed_time(300,300,0b11,2000);
    delay(2000);*/
    //fahre_bis_kreuzung();
    
  //}else{
  //  fahre_bis_kreuzung();
  //}
  //counter++;
  
  //Serial.
  //lese_bitfeld();
  
  //Serial.println(sensor1);
  //Serial.println(sensor2);
  /*for(uint8_t i=0; i<3; i++){
    Serial.print(bitwerte[i]);
  }*/
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
