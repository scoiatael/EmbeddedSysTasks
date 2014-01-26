
const int Yellow = 2;
const int Red = 3;
const int Green = 4;

const int ledPinsSize = 3;
// Yellow, Red, Green
const int ledPins[ledPinsSize] =  {Yellow,Red,Green};      // the number of the LED pin

void setupLeds() {
  for(int i=0; i<ledPinsSize; i++)
    pinMode(ledPins[i], OUTPUT);      
}

void lightLed(int ledNr)
{
//assert(ledNr <= ledPins[ledPinsSize-1] && ledNr >= ledPins[0]);  
  digitalWrite(ledPins[ledNr], HIGH);
}

void shutLed(int ledNr)
{
//assert(ledNr <= ledPins[ledPinsSize-1] && ledNr >= ledPins[0]);  
  digitalWrite(ledPins[ledNr], LOW);
}  

void blinkLed(int ledNr)
{
  lightLed(ledNr);
  delay(100);
  shutLed(ledNr);
}

int curLedInd;

void loop()
{
  curLedInd++;
  if(curLedInd>=ledPinsSize)                                                   
    curLedInd=0;
  lightLed(curLedInd);
  delay(500);
  shutLed(curLedInd);
  Serial.println(curLedInd);
  Serial.println(ledPins[curLedInd]);
}

void setup()
{
  setupLeds();
  curLedInd=0;
}
