/*

   1          2

       3 4       
MA               MB 
*/
enum {ACCEL, DECEL, CONST, CW, CCW};

#define STEP_A PD2
#define STEP_B PB0

//magic numbers die getestet werden müssen:
uint16_t cruise_speed_lev1 = 500;
uint16_t turn_speed_lev1 = 100;
uint16_t schwarz_schwellenwert = 700;
uint16_t drehzeit_ms = 2000;
uint16_t steps_bis_kreuzung = 5;

uint16_t sensor1 = sensorwerte(1);
uint16_t sensor2 = sensorwerte(2);
uint16_t sensor3 = sensorwerte(3);
uint16_t sensor4 = sensorwerte(4);

static volatile uint16_t bitwerte[3];

void fahre_speed_time(uint16_t v_mA, uint16_t vmB, uint16_t ms){
  motorA_speed(v_mA);
  motorB_speed(v_mB);
  
  delay(ms);
  
  motorA_speed(0);
  motorB_speed(0);
}

void fahre_steps(uint16_t steps){
  for(uint16_t i = 0; i < steps; i++){
    STEP_A = 1;
    STEP_B = 1;
    STEP_A = 0;
    STEP_B = 0;
  }
}

void fahre(){

  if (sensor3 < sensor4){
      
      motorA_speed(cruise_speed_lev1 + 1);
      motorB_speed(cruise_speed_lev1);
      
    }else if(sensor3 > sensor4){
      
      motorA_speed(cruise_speed_lev1);
      motorB_speed(cruise_speed_lev1 + 1);
    
    }
  
}

void fahre_bis_bitfeld(){
  motorA_speed(cruise_speed_lev1);
  motorB_speed(cruise_speed_lev1);
  
  while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){
    
    fahre();
    
  }
  motorA_speed(0);
  motorB_speed(0);
  
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

    fahre_bis_bitfeld();

    if (sensor1 > schwarz_schwellenwert){
    
      bitwerte_1[i] = bitwerte_1[i] + 2;
  
    }else if (sensor2 > schwarz_schwellenwert){
    
      bitwerte_2[i] = bitwerte_2[i] + 2;
  
    }

    fahre_bis_bitfeld();

    if (sensor1 > schwarz_schwellenwert){
    
      bitwerte_1[i] = bitwerte_1[i] + 4;
  
    }else if (sensor2 > schwarz_schwellenwert){
    
      bitwerte_2[i] = bitwerte_2[i] + 4;
  
    }

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
  
  while(sensor1 < schwarz_schwellenwert && sensor2 < schwarz_schwellenwert){
    
    fahre();
    
  }
  motorA_speed(0);
  motorB_speed(0);
  
}

void drehe_rechts(){
  /*
  while(sensor3 < schwarz_schwellenwert || sensor4 < schwarz_schwellenwert){
  
    motorA_speed(turn_speed_lev1);
    motorB_speed(turn_speed_lev1);
  }
  */
  fahre_steps(steps_bis_kreuzung);
  
  motorA_speed(0);
  motorB_speed(0);
  
  fahre_steps(turn_speed_lev1, (-turn_speed_lev1), drehzeit_ms);

  while(sensor3 > schwarz_schwellenwert && sensor4 > schwarz_schwellenwert){

    if (sensor1 < sensor2){
      
      motorA_speed(turn_speed_lev1 + 1);
      motorB_speed(turn_speed_lev1);
      
    }else if(sensor1 > sensor2){
      
      motorA_speed(turn_speed_lev1);
      motorB_speed(turn_speed_lev1 + 1);
    
    }

  }

    motorA_speed(0);
    motorB_speed(0);
  
}

void drehe_links(){
  /*
  while(sensor3 < schwarz_schwellenwert || sensor4 < schwarz_schwellenwert){
  
    motorA_speed(turn_speed_lev1);
    motorB_speed(turn_speed_lev1);
  }
  */
  fahre_steps(steps_bis_kreuzung);
  
  motorA_speed(0);
  motorB_speed(0);
  
  fahre_steps((-turn_speed_lev1), turn_speed_lev1, drehzeit_ms);

  while(sensor3 > schwarz_schwellenwert && sensor4 > schwarz_schwellenwert){

    if (sensor1 < sensor2){
      
      motorA_speed(turn_speed_lev1 + 1);
      motorB_speed(turn_speed_lev1);
      
    }else if(sensor1 > sensor2){
      
      motorA_speed(turn_speed_lev1);
      motorB_speed(turn_speed_lev1 + 1);
    
    }

  }

    motorA_speed(0);
    motorB_speed(0);
  
}


void setup() {


}

void loop() {
  // put your main code here, to run repeatedly:

}
