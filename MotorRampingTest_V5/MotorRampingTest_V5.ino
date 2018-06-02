/*
  TIMER2 , PORTD , PORTB

  Timer2 wird im CTC Modus benutz und läuft auf 160KHz und führt dann die ISR(TIMER2_COMPA_vect) aus.

  Alle Methoden (außer motor_init) existieren für beide Motoren, also das A durch B ersetzen um den anderern Motor anzusprechen

  I/O:
    - Motor A wird durch PORT D gesteuert:
      - belegte Pins:
        - PD2 -> Step
        - PD3 -> Direction
        - PD4 -> Microstepping M0
        - PD5 -> Microstepping M1
        - PD6 -> Enable
    - Motor B wird durch PORT B gesteuert:
      - belegte Pins:
        - PD0 -> Step
        - PD1 -> Direction
        - PD2 -> Microstepping M0
        - PD3 -> Microstepping M1
        - PD4 -> Enable

  Variablen:
    - enums
      - ACCEL -> Beschleunigend
      - DECEL -> Bremsend
      - CONST -> konstant
      - CW    -> clockwise
      - CCW   -> counter clockwise
    - vBase:
        Basisgeschwindigkeit, bzw. jetzige geschwindigkeit
    - vTarget:
        Zielgeschwindigkeit, bzw. Geschwindigkeit auf die beschleunigt wird
    - acc:
        Beschleunigung des Motors als absoluter Wert
    - del_init:
        delay des ersten Intervalls, wird einmalig nach setzten der neuen Geschwindigkeit errechnet
    - acceleration:
        Beschleunigung des Motors als beschleunigend, bremsend oder konstant
    - r: (evtl in die set_acc Methode verlagern) 
        konstanter Multiplikator, wird einmalig nach setzten der neuen Geschwindigkeit errechnet
    - steps:
        Anzahl an steps um von jetziger Geschwindigkeit auf die Zielgeschwindigkeit zu kommen
    - del_current:
        momentaner delay bis zum nächsten step
    - step_counter:
        counter zum Mitzählen der Schritte
    - del_counter:
        counter zum Mitzählen des delays
    
  Methoden:
    - motor_init()
        Initialisiert die DDR Register für die Motoren
        Konfiguriert den Timer 2
        Setzt Frequenz auf 160KHz
        Setzt die Motordrehrichtungen beider Motoren auf Clockwise
        Setzt die Motorbeschleunigung auf konstant  

!! im moment nicht ändern wenn motor dreht, sonst änder er direkt die Richtung und würde stallen
!! mögl lösung: beim setzten neuer Richtung auf null bremsen dann Richtung änern und dann auf gleichn Wert wieder beschleunigen
!! aber nicht präzise da änderung nicht sofort und Situation kommt bei uns nicht vor    
    - motorA_direction(uint8_t dir)
        benötigt die enums CW und CCW
        setz Drehrichtung des Motors auf CW oder CCW

    - motorA_speed(uint16_t target)
        benötigt neue gewünschte Geschwindigkeit
        setzt neue Geschwindigkeit
        Änderung erfolgt sofort mit gesetzter Beschleunigung

    - motorA_acceleration(float acc)
        benötigt neue gewünschte Beschleunigung
        setzt neue Beschleunigung, nächste Geschwindigkeitsänderung erfolgt dann mit neuer Beschleunigung

    - motorA_microStep(uint8_t stepSize)
        benötigt gewünschte Schrittgröße: 1, 2, 4, 8, 16, 32
        setzt sofort neue Schrittgröße

!! falls Geschwindigkeit vor enable gesetz beginnt die Software schon zu beschleunigen ohne dass der Motor mitdreht
!! somit wird der Motor eventuell stallen wenn mann ihn dann später aktiviert
    - motorA_enable()
        aktiviert Motor
        Motor bekommt maximalen Haltestrom wenn er still steht, kann also überhitzen

    - motorA_disable()
        deaktiviert Motor
        Motor hat keinen Haltestrom mehr, läuft freigängig, würde also wenn er Geschwindigkeit hat ausrollen 
      
    - ISR()
        steuert beide Motoren
        macht falls nötig einen step
        berechnet neuen delay für den nächsten step
*/

enum {ACCEL, DECEL, CONST, CW, CCW};
float mc_timerFrequenz;

// Motor A
#define DDR_A DDRD
#define PORT_A PORTD
#define STEP_A PD2
#define DIR_A PD3
#define M0_A PD4
#define M1_A PD5
#define ENABLE_A PD6
float mc_motorA_vBase;
float mc_motorA_vTarget;
float mc_motorA_acc;
float mc_motorA_del_init;
volatile uint8_t mc_motorA_acceleration;
volatile float mc_motorA_r;
volatile float mc_motorA_steps;
volatile float mc_motorA_del_current;
volatile int mc_motorA_step_counter;
volatile int mc_motorA_del_counter;


// Motor B
#define DDR_B DDRB
#define PORT_B PORTB
#define STEP_B PB0
#define DIR_B PB1
#define M0_B PB2
#define M1_B PB3
#define ENABLE_B PB4
float mc_motorB_vBase;
float mc_motorB_vTarget;
float mc_motorB_acc;
float mc_motorB_del_init;
volatile uint8_t mc_motorB_acceleration;
volatile float mc_motorB_r;
volatile float mc_motorB_steps;
volatile float mc_motorB_del_current;
volatile int mc_motorB_step_counter = 0;
volatile int mc_motorB_del_counter = 0;

uint32_t s = 50000;
uint32_t a = 1000000;

void setup() {
  motor_init();

  motorA_enable();
  motorA_microStep(16);
  
  motorB_enable();
  motorB_microStep(16);

  motorA_acceleration(a);
  motorB_acceleration(a);

  sei();
}

void loop() {
  motorA_direction(CW);
  motorA_speed(s);
  motorB_direction(CW);
  motorB_speed(s);
  delay(500);
  motorA_speed(0);
  motorB_speed(0);
  delay(500);
  motorA_direction(CCW);
  motorA_speed(s);
  motorB_direction(CW);
  motorB_speed(s);
  delay(250);
  motorA_speed(0);
  motorB_speed(0);
  delay(500);
}

ISR(TIMER2_COMPA_vect) {
  //check if del_counter is high enough, higher than del_current, for the next step
  if (mc_motorA_del_counter > mc_motorA_del_current) {
    //reset del_counter to zero
    mc_motorA_del_counter = 0;
    //check if target velocity is zero and motor acceleration is constant
    if (mc_motorA_vTarget == 0 && mc_motorA_acceleration == CONST) {
      //do nothing so the motor stands still
    } else {
      //pull step pin high for the next step
      PORT_A |= (1 << STEP_A);
    }
    //check if step_counter (already made steps) is lower than steps (total steps to make)
    if (mc_motorA_step_counter < mc_motorA_steps) {
      //check if motor acceleration is accelerating ore decelerating and then calculate the next step delay
      if (mc_motorA_acceleration == ACCEL) {
        mc_motorA_del_current = mc_motorA_del_current * ( 1 + (-mc_motorA_r) * mc_motorA_del_current * mc_motorA_del_current);
      } else if (mc_motorA_acceleration == DECEL) {
        mc_motorA_del_current = mc_motorA_del_current * ( 1 + (mc_motorA_r) * mc_motorA_del_current * mc_motorA_del_current);
      }
      //increase step counter
      mc_motorA_step_counter++;
    } else {
      //if step counter is lower, then set acceleration to constant
      mc_motorA_acceleration = CONST;
    }
    //pull step pin low
    PORT_A &= ~(1 << STEP_A);
  }
  //increase delay counter
  mc_motorA_del_counter ++;


  if (mc_motorB_del_counter > mc_motorB_del_current) {
    mc_motorB_del_counter = 0;
    if (mc_motorB_vTarget == 0 && mc_motorB_acceleration == CONST) {
    } else {
      PORT_B |= (1 << STEP_B);
    }
    if (mc_motorB_step_counter < mc_motorB_steps) {
      if (mc_motorB_acceleration == ACCEL) {
        mc_motorB_del_current = mc_motorB_del_current * ( 1 + (-mc_motorB_r) * mc_motorB_del_current * mc_motorB_del_current);
      } else if (mc_motorB_acceleration == DECEL) {
        mc_motorB_del_current = mc_motorB_del_current * ( 1 + (mc_motorB_r) * mc_motorB_del_current * mc_motorB_del_current);
      }
      mc_motorB_step_counter++;
    } else {
      mc_motorB_acceleration = CONST;
    }
    PORTB &= ~(1 << STEP_B);
  }
  mc_motorB_del_counter ++;
}

void motor_init() {
  //set pins as output
  DDR_A |= ((1 << STEP_A) | (1 << DIR_A) | (1 << ENABLE_A));
  DDR_B |= ((1 << STEP_B) | (1 << DIR_B) | (1 << ENABLE_B));

  //configure timer 2
  TCCR2A = 0b00000010;
  TCCR2B = 0b00000001;
  TIMSK2 = 0b00000010;
  OCR2A = 100;

  //set frequency to 160KHz
  mc_timerFrequenz = 160000;

  //initial direction and acceleration of both motors
  motorA_direction(CW);
  motorB_direction(CW);
  mc_motorA_acceleration = CONST;
  mc_motorB_acceleration = CONST;
}

void motorA_direction(uint8_t dir) {
  if (dir == CW) {
    PORT_A |= 1 << DIR_A;
  } else {
    PORT_A &= ~(1 << DIR_A);
  }
}

void motorB_direction(uint8_t dir) {
  if (dir == CCW) {
    PORT_B |= 1 << DIR_B;
  } else {
    PORT_B &= ~(1 << DIR_B);
  }
}

void motorA_speed(uint16_t target) {
  //disable interrupts
  cli();
  //set base velocity to last target velocity
  mc_motorA_vBase = mc_motorA_vTarget;
  //set target velocity to new velocity
  mc_motorA_vTarget = target;
  //check is target velocity is higher than base velocity
  if (mc_motorA_vTarget > mc_motorA_vBase) {
    //set acceleration to accerlerating
    mc_motorA_acceleration = ACCEL;
    //check is target velocity is lower than base velocity
  } else if (mc_motorA_vTarget < mc_motorA_vBase) {
    //set acceleration to decerlerating
    mc_motorA_acceleration = DECEL;
  } else {
    //else set acceleration to constant
    mc_motorA_acceleration = CONST;
  }
  //check if acceleration is not constant
  if (mc_motorA_acceleration != CONST) {
    //calculate the multiplier for the set acceleration
    mc_motorA_r = mc_motorA_acc / (mc_timerFrequenz * mc_timerFrequenz);
    //calculate initial delay
    mc_motorA_del_init = mc_timerFrequenz / sqrt((mc_motorA_vBase * mc_motorA_vBase) + 2 * mc_motorA_acc);
    //check if acceleration is accelerating
    if (mc_motorA_acceleration == ACCEL) {
      //calculate the first delay from inital delay
      mc_motorA_del_current = mc_motorA_del_init * ( 1 + (-mc_motorA_r) * mc_motorA_del_init * mc_motorA_del_init);
      //calculate steps to make during speed transition
      mc_motorA_steps = ((mc_motorA_vTarget * mc_motorA_vTarget) - (mc_motorA_vBase * mc_motorA_vBase)) / (2 * mc_motorA_acc);
      //check if acceleration is decelerating
    } else if (mc_motorA_acceleration == DECEL) {
      //calculate the first delay from inital delay
      mc_motorA_del_current = mc_motorA_del_init * ( 1 + (mc_motorA_r) * mc_motorA_del_init * mc_motorA_del_init);
      //calculate steps to make during speed transition
      mc_motorA_steps = ((mc_motorA_vBase * mc_motorA_vBase) - (mc_motorA_vTarget * mc_motorA_vTarget)) / (2 * mc_motorA_acc);
    }
    //reset step counter
    mc_motorA_step_counter = 0;
  }
  sei();
}

void motorB_speed(uint16_t target) {
  cli();
  mc_motorB_vBase = mc_motorB_vTarget;
  mc_motorB_vTarget = target;
  if (mc_motorB_vTarget > mc_motorB_vBase) {
    mc_motorB_acceleration = ACCEL;
  } else if (mc_motorB_vTarget < mc_motorB_vBase) {
    mc_motorB_acceleration = DECEL;
  } else {
    mc_motorB_acceleration = CONST;
  }
  if (mc_motorB_acceleration != CONST) {
    mc_motorB_r = mc_motorB_acc / (mc_timerFrequenz * mc_timerFrequenz);
    mc_motorB_del_init = mc_timerFrequenz / sqrt((mc_motorB_vBase * mc_motorB_vBase) + 2 * mc_motorB_acc);
    if (mc_motorB_acceleration == ACCEL) {
      mc_motorB_del_current = mc_motorB_del_init * ( 1 + (-mc_motorB_r) * mc_motorB_del_init * mc_motorB_del_init);
      mc_motorB_steps = ((mc_motorB_vTarget * mc_motorB_vTarget) - (mc_motorB_vBase * mc_motorB_vBase)) / (2 * mc_motorB_acc);
    } else if (mc_motorB_acceleration == DECEL) {
      mc_motorB_del_current = mc_motorB_del_init * ( 1 + (mc_motorB_r) * mc_motorB_del_init * mc_motorB_del_init);
      mc_motorB_steps = ((mc_motorB_vBase * mc_motorB_vBase) - (mc_motorB_vTarget * mc_motorB_vTarget)) / (2 * mc_motorB_acc);
    }
    mc_motorB_step_counter = 0;
  }
  sei();
}

void motorA_acceleration(float acc) {
  mc_motorA_acc = acc;
}

void motorB_acceleration(float acc) {
  mc_motorB_acc = acc;
}

void motorA_microStep(uint8_t stepSize) {
  switch (stepSize) {
    case 1:
      DDR_A |= (1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A &= ~(1 << M0_A);
      PORT_A &= ~(1 << M1_A);
      break;
    case 2:
      DDR_A |= (1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A |= (1 << M0_A);
      PORT_A &= ~(1 << M1_A);
      break;
    case 4:
      DDR_A &= ~(1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A &= ~(1 << M0_A);
      PORT_A &= ~(1 << M1_A);
      break;
    case 8:
      DDR_A |= (1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A &= ~(1 << M0_A);
      PORT_A |= (1 << M1_A);
      break;
    case 16:
      DDR_A |= (1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A |= (1 << M0_A);
      PORT_A |= (1 << M1_A);
      break;
    case 32:
      DDR_A &= ~(1 << M0_A);
      DDR_A |= (1 << M1_A);
      PORT_A &= ~(1 << M0_A);
      PORT_A |= (1 << M1_A);
      break;
  }
}

void motorB_microStep(uint8_t stepSize) {
  switch (stepSize) {
    case 1:
      DDR_B |= (1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B &= ~(1 << M0_B);
      PORT_B &= ~(1 << M1_B);
      break;
    case 2:
      DDR_B |= (1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B |= (1 << M0_B);
      PORT_B &= ~(1 << M1_B);
      break;
    case 4:
      DDR_B &= ~(1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B &= ~(1 << M0_B);
      PORT_B &= ~(1 << M1_B);
      break;
    case 8:
      DDR_B |= (1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B &= ~(1 << M0_B);
      PORT_B |= (1 << M1_B);
      break;
    case 16:
      DDR_B |= (1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORTB |= (1 << M0_B);
      PORTB |= (1 << M1_B);
      break;
    case 32:
      DDR_B &= ~(1 << M0_B);
      DDR_B |= (1 << M1_B);
      PORT_B &= ~(1 << M0_B);
      PORT_B |= (1 << M1_B);
      break;
  }
}

void motorA_enable() {
  PORT_A |= (1 << ENABLE_A);
}

void motorB_enable() {
  PORT_B |= (1 << ENABLE_B);
}

void motorA_disable() {
  PORT_A &= ~(1 << ENABLE_A);
}

void motorB_disable() {
  PORT_B &= ~(1 << ENABLE_B);
}

