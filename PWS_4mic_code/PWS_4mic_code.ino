#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

void setup() {
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  pinMode(LED_BUILTIN,OUTPUT);
  /* Initialiseer Intercepts:
    als pin 4,5,6 of 7 geactiveerd wordt, voer dan de betreffende functie uit.
    sneller dan in loop controleren of microfoon geactiveerd is.
    timing blijft behouden door "intercept wachtrij"
  */
  attachPCINT(digitalPinToPCINT(4),AInterrupt,CHANGE);
  attachPCINT(digitalPinToPCINT(5),BInterrupt,CHANGE);
  attachPCINT(digitalPinToPCINT(6),CInterrupt,CHANGE);
  attachPCINT(digitalPinToPCINT(7),DInterrupt,CHANGE);
  //start communicatie met computer via USB-kabel
  Serial.begin(9600);
}

//volatile: kan in intercept veranderd worden
//unsigned long: type die macros() teruggeeft, "overflow" na ~70 min.
volatile unsigned long ALow = -1;
volatile unsigned long BLow = -1;
volatile unsigned long CLow = -1;
volatile unsigned long DLow = -1;
const float RX = 0.985;
const float RY = 0.975;
const float RZ = 0.985;
unsigned long Time;

//Als tijd ongedefinieerd:
//Zet tijd als huidige tijd
void AInterrupt(){
  ALow = micros();
  disablePCINT(digitalPinToPCINT(4));
}

void BInterrupt(){
  BLow = micros();
  disablePCINT(digitalPinToPCINT(5));
}

void CInterrupt(){
  CLow = micros();
  disablePCINT(digitalPinToPCINT(6));
}

void DInterrupt(){
  DLow = micros();
  disablePCINT(digitalPinToPCINT(7));
}

void loop() {
  Time = micros();
  // Als alle microfoons een seconde geleden zijn geactiveerd dan
  if (ALow != -1 and 
      BLow != -1 and 
      CLow != -1 and 
      DLow != -1 and 
      Time-ALow > 1000000 and 
      Time-BLow > 1000000 and
      Time-CLow > 1000000 and
      Time-DLow > 1000000)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    //print de verschillen
    //snelheid geluid x verschil in aankomsttijd (s)
    float DX = (340.29*((float)BLow-(float)ALow)/1000000.0);
    float DY = (340.29*((float)CLow-(float)ALow)/1000000.0);
    float DZ = (340.29*((float)DLow-(float)ALow)/1000000.0);
//    Serial.print(DX);
//    Serial.print(',');
//    Serial.print(DY);
//    Serial.print(',');
//    Serial.print(DZ);
//    Serial.print(',');
    if (DX > -RX and DX < RX and DY > -RY and DY < RY and DZ > -RZ and DZ < RZ){
      float TCalA = (1-(pow(DX,2)/pow(RX,2))-(pow(DY,2)/pow(RY,2))-(pow(DZ,2)/pow(RZ,2)));
      float TCalB = -((pow(DX,3)/pow(RX,2))+(pow(DY,3)/pow(RY,2))+(pow(DZ,3)/pow(RY,2))-DX-DY-DZ);
      float TCalC = -(pow((pow(DX,2)-pow(RX,2))/(2*pow(RX,2)),2)+pow((pow(DY,2)-pow(RY,2))/(2*pow(RY,2)),2)+pow((pow(DZ,2)-pow(RZ,2))/(2*pow(RZ,2)),2));
//      Serial.print(TCalA);
//      Serial.print(',');
//      Serial.print(TCalB);
//      Serial.print(',');
//      Serial.print(TCalC);
//      Serial.print(',');
      float T1 = (-TCalB+sqrt(pow(TCalB,2)-4*TCalA*TCalC))/(2*TCalA);
      float T2 = (-TCalB-sqrt(pow(TCalB,2)-4*TCalA*TCalC))/(2*TCalA);
//      Serial.println(T1);
//      Serial.print('\n');
      Serial.print((pow(RX,2)-2*T1*DX-pow(DX,2))/(2*RX));
      Serial.print('\t');
      Serial.print((pow(RY,2)-2*T1*DY-pow(DY,2))/(2*RY));
      Serial.print('\t');
      Serial.print((pow(RZ,2)-2*T1*DZ-pow(DZ,2))/(2*RZ));
      Serial.print('\n');
    }
    else{
      Serial.println();
    }
    digitalWrite(LED_BUILTIN,LOW);
  }
  if (
      (Time-ALow > 1000000 or ALow == -1) and 
      (Time-BLow > 1000000 or BLow == -1) and 
      (Time-CLow > 1000000 or CLow == -1) and 
      (Time-DLow > 1000000 or DLow == -1))
  {
    //reset ALow-DLow en heractiveer alle interrupts
    ALow = -1;
    BLow = -1;
    CLow = -1;
    DLow = -1;
    enablePCINT(digitalPinToPCINT(4));
    enablePCINT(digitalPinToPCINT(5));
    enablePCINT(digitalPinToPCINT(6));
    enablePCINT(digitalPinToPCINT(7));
  }
}

