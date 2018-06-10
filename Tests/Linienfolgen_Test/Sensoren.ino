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

volatile uint16_t buffer_sensorwerte[11];
volatile uint8_t sc_wert_counter = 0;

void sc_setup() {
  ADC_DDR &= ~(1<<HELL1);
  ADC_DDR &= ~(1<<HELL2);
  ADC_DDR &= ~(1<<ABSTAND);
  //Multi:
  ADC_DDR &= ~(1<<MPX_Z);
  MPX_DDR |= (1<<MPX_S0);
  MPX_DDR |= (1<<MPX_S1);
  MPX_DDR |= (1<<MPX_S2);

  ADCSRA = 0b10001111;
  ADCSRB = 0b00000000;
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
  }  
  ADCSRA |= (1 << ADSC);
}
