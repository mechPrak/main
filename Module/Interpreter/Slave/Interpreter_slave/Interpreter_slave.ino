enum {ACCEL, DECEL, CONST, CW, CCW}; //Enum für Motoren
volatile uint8_t MotorTimerEvent=0; //Eventvariable für den Motortimer
volatile uint8_t i2cNewData=0; //Eventvariable für i2c
volatile uint16_t *i2c_16BitArray; //Array, aus dem die Werte über i2c gelesen werden.
/*Übertragen wird über i2c:
 * Index 0: Geschwindigkeit von Motor A
 * Index 1: Geschwindigkeit von Motor B
 * Index 2: Bit0: 0->CW, 1->CCW (MotorA)
 *          Bit1: 0->CW, 1->CCW (MotorB)
 */


void setup() {
  // put your setup code here, to run once:
  motor_setup(); //setup() vom Motor-Modul
  i2c_setup(); //setup() vom i2c-Modul
  Serial.begin(9600);
}

void loop() {
  if(i2cNewData){
    
    delay(100);
    i2cNewData=0;
    motorA_speed(i2c_16BitArray[0]);

    motorB_speed(i2c_16BitArray[1]);
   
    
    if(i2c_16BitArray[2] & 0b01){
      motorA_direction(CCW);
    }
    else{
      motorA_direction(CW);
    }
    if(i2c_16BitArray[2] & 0b10){
      motorB_direction(CCW);
    }
    else{
      motorB_direction(CW);
    }
  }
  if(MotorTimerEvent){
    //Nachbilden des Interrupts für die Motoren
    MotorTimerEvent=0;
    motorTimer();
  }
}
