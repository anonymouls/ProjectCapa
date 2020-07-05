/*
 Name:    ProtoCapa.ino
 Created: 3/6/2020 4:06:08 AM
 Author:  RedCrown
*/

int MeasurePinX1 = 13; // PORTB5
int MeasurePinY1 = 2;

int ButtonPin = 3;

bool MeasureMode = false;
bool SetUp = false;
volatile unsigned long StartValue = -1;
volatile unsigned long TickCounter = 0;
volatile byte ADCValue;
long ProtectionValue = 1000000;// 1 second ? millis=10-3 micros=10-6


ISR (TIMER1_OVF_vect)
{
  TickCounter++;
}

void LauchTimer(){
  noInterrupts();
  TIMSK1=0x01; // enabled global and timer overflow interrupt;
  TCCR1A = 0x00; // normal operation page 148 (mode0);
  TCNT1=0x0000; // 16bit counter register
  TCCR1B = 1; // start timer/ set clock
  interrupts();
}

void ButtonCatched(){
  if (MeasureMode){
    Serial.println("Exiting From Measuring");
    StartValue = -1;
    TCCR1B = 0;
    MeasureMode = false;
    PORTB &= ~(1 << PORTB5);
    delay(2000);
  } else {
    MeasureMode = true;
    TickCounter = 0;
    PORTB |= (1 << PORTB5);
    LauchTimer();
  }
}

void MeasureEnded(){
  noInterrupts();
  TCCR1B = 0;
  unsigned long TimerTicks = TCNT1;
  PORTB &= ~(1 << PORTB5);
  Serial.print("Overflows: "); Serial.println(TickCounter);
  Serial.print("Current ticks: "); Serial.println(TimerTicks);
  TimerTicks = TimerTicks+ (TickCounter*65535);
  Serial.print("Overall ticks: "); Serial.println(TimerTicks);
  Serial.print("Recoded Value ticks edition: ");
  double Result = TimerTicks*62.5;
  Serial.println(Result);
  MeasureMode = false;
  StartValue = -1;
  interrupts();
  //ButtonCatched();
}

void setup() {           
  //SetupADC();
  Serial.begin(9600);
  pinMode(MeasurePinX1, OUTPUT);
  pinMode(MeasurePinY1, INPUT); 
  PORTB &= ~(1 << PORTB5);
  attachInterrupt(digitalPinToInterrupt(MeasurePinY1), MeasureEnded, RISING);
  
  //Serial.println("Set up complete");
  //Serial.println("Launching");
  ButtonCatched();
}

void loop() {

}
