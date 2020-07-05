#include <bcconfig.h>
#include <number.h>

#define analogPin      0          
#define chargePin      13         
#define dischargePin   11        

volatile unsigned long TimerInterrupts = 0; // todo optimize
unsigned long ResistorValue = 1200000;
const unsigned long NanosToSeconds = 10000000;
unsigned long Nanos = 0;

ISR (TIMER1_OVF_vect){  TimerInterrupts++;  }

void setup(){
  pinMode(chargePin, OUTPUT);     
  digitalWrite(chargePin, LOW);  
  Serial.begin(9600);             
}

void LaunchTimer(){
  noInterrupts();
  TimerInterrupts = 0;
  Nanos = 0;
  TIMSK1=0x01; // enabled global and timer overflow interrupt;
  TCCR1A = 0x00; // normal operation page 148 (mode0);
  TCNT1=0x0000; // 16bit counter register
  TCCR1B = 1; // start timer/ set clock
  interrupts();
}

void StopTimer(){
  noInterrupts();
  TCCR1B = 0;
  Nanos = (TimerInterrupts*65535)+TCNT1;
    Serial.print("Timer Interrupts: "); Serial.println(TimerInterrupts);
//    Serial.print("Nanos: "); Serial.println(Nanos);
  interrupts();
}

void loop(){
  digitalWrite(chargePin, HIGH);  
  LaunchTimer();
  while(analogRead(analogPin) < 648){       
  }
  StopTimer();
  double Regression = 0.6;
  double Farads = ((double)Nanos/(double)ResistorValue/(double)NanosToSeconds)*Regression;
  Serial.print("Nanos: "); Serial.println(Nanos);
  Serial.print("Micros: "); Serial.println(((double)Nanos)/1000.0);
  if (Farads < 1){
    Farads *= 1000;
    if (Farads > 1) {
      Serial.print(Farads); Serial.println(" miliFarads");         
    } else {
      Farads *= 1000;
      if (Farads > 1) {
        Serial.print(Farads); Serial.println(" microFarads");         
      } else {
        Farads *= 1000;
        if (Farads > 1){
          Serial.print(Farads); Serial.println(" nanoFarads");         
        } else{
          Farads *= 1000;
          if (Farads > 1){
            Serial.print(Farads); Serial.println(" pikoFarads");         
          } else {
            Serial.print(Farads); Serial.println(" <pikoFarads?");
          }
        }
      }
    }
  }

  else{
    Serial.print((double)Farads);         
    Serial.println(" Farads");          
    delay(500); 
  }

  digitalWrite(chargePin, LOW);            
  pinMode(dischargePin, OUTPUT);            
  digitalWrite(dischargePin, LOW);          
  while(analogRead(analogPin) > 0){         
  }
  pinMode(dischargePin, INPUT);            
  delay(2000);
}
