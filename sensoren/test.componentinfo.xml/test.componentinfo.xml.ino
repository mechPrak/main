// ACHTUNG ACHTUNG SERIELLER MONITOR IST NOCH DRIN!!
/*
Register:
  ADMUX
  ADC
  DDRC
  DDRD
  ADCSRA
  ADCSRB
  PRTD
 
Pins:
  PC0
  PC1
  PC2
  PC3
  PD4
  PD3
  PD2
+++++Schnittstelle+++++
Aufruf durch Methode:
sc_refreshValues()  //schreibt aktuelle Sensorwerte ins Array "sensorwerte"
Array[11] "sensorwerte"
1.  Hallsense
2.  Helligkeitssensor vorne mitte
3.  Helligkeitssensor mitte links
4.  Helligkeitssensor mitte rechts
5.  Helligkeitssensor hinten
6.  Helligkeitssensor vorne links
7.  Helligkeitssensor mitte mitte
8.  Helligkeitssensor vorne rechts
9.  Helligkeitssensor links
10. Helligkeitssensor rechts
11. Abstandssensor
+++++Schnittstelle+++++
*/
#define MPX_Z PC0
#define MPX_S0 PD4
#define MPX_S1 PD3
#define MPX_S2 PD2
#define MPX_PORT PORTD
#define HELL1 PC1
#define HELL2 PC2
#define ABSTAND PC3
#define ADC_DDR DDRC
#define MPX_DDR DDRD

static volatile uint16_t sensorwerte[11];
static volatile uint16_t buffer_sensorwerte[11];
static volatile uint8_t sc_wert_counter = 0;

void sc_setup() {
  Serial.begin(9600);
 
  ADC_DDR &= ~(1<<HELL1);
  ADC_DDR &= ~(1<<HELL2);
  ADC_DDR &= ~(1<<ABSTAND);
  //Multi:
  ADC_DDR &= ~(1<<MPX_Z);
  MPX_DDR |= (1<<MPX_S0);
  MPX_DDR |= (1<<MPX_S1);
  MPX_DDR |= (1<<MPX_S2);
  //ADMUX = 0b01000000; // ADC0
  ADCSRA = 0b10001111; //vorher: 0b10100111 2. 0b10101011;
  ADCSRB = 0b00000000;
  //ADCSRA |= (1 << ADSC); //ADC starten
  sei();
}

void sc_refreshValues() {
  cli();
  for(uint8_t i = 0; i < 11; i++){
    sensorwerte[i] = buffer_sensorwerte[i];
  }
  sc_wert_counter = 0;
  ADMUX = 0b01000010; // ADC2
  MPX_PORT = (sc_wert_counter << 2);
  ADCSRA |= (1 << ADSC);
  sei();

 /* 
  ADMUX = 0b01000000; // ADC0
  ADCSRA |= (1 << ADSC);
  
  ADMUX = 0b01000001; // ADC1
  ADCSRA |= (1 << ADSC);
  
  ADMUX = 0b01000011; // ADC3
  ADCSRA |= (1 << ADSC);
  */
}

ISR(ADC_vect){
  buffer_sensorwerte[sc_wert_counter] = ADC;
  if (sc_wert_counter < 7){
    sc_wert_counter++;
    MPX_PORT = (sc_wert_counter << 2);
  
  }else if (sc_wert_counter == 7){
    sc_wert_counter++;
    ADMUX = 0b01000000; // ADC0
  
  }else if (sc_wert_counter == 8){
    sc_wert_counter++;
    ADMUX = 0b01000001; // ADC1
    
  }else if (sc_wert_counter == 9){
    sc_wert_counter++;
    ADMUX = 0b01000011; // ADC3
    
  }else if (sc_wert_counter == 10){
    sc_wert_counter++;
    sc_refreshValues();
    cli();
    for(uint8_t j = 0; j<11;j++){
      Serial.print(sensorwerte[j]);
      Serial.print(" ");
    }
    Serial.println();
    sei();
  }
  
  ADCSRA |= (1 << ADSC);
}

void setup(){
  sc_setup();
  sc_refreshValues();
}
void loop(){
  /*sc_refreshValues();
  for(uint8_t j = 0; j<11;j++){
      Serial.print(sensorwerte[j]);
      Serial.print(" ");
  }
    Serial.println();
    sei();*/
  }

