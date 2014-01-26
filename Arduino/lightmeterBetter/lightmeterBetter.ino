//Digital
const int LEDo=9;
//Analog
const int In=0;
const int P1=1;
const int P2=2;

const int READSEACHTIME=3;

const int COUNT=1000;
int count=0;
unsigned long sum=0;

const int TIMEOUT=1000;
int timeout=0;

int cmax=255;
int cmin=0;

int val=0;

void setup()
{
  Serial.begin(9600);
}

void setMin()
{
  cmin=val;
}

void setMax()
{
  cmax=val;
}

void resetCount()
{
  val = sum/COUNT/4;
  count=0;
  sum=0;
}

void incrementCount(int v)
{
  count++;
  sum = sum+v;
  if(count >= COUNT)
    resetCount();
}

int severalAnalogReads(int l)
{
  unsigned long s=0;
  for(int i=0; i<READSEACHTIME; i++)
    s+=analogRead(l);
  return s/READSEACHTIME;
}

int pollButt(int b)
{
  return analogRead(b)>=1000;
}

int displayVal(int v)
{
  if(v>=cmax)
    v=cmax;
  v = v-cmin;
  if(v<=0)
    return 0;
  return v;
}

void printState()
{
  if(timeout>0)
  {
    Serial.println("Ignored");
    return;
  }
  Serial.print("val: "); Serial.println(val);  
  Serial.print("cmin: "); Serial.println(cmin);  
  Serial.print("cmax: "); Serial.println(cmax);  
}

void incrementTimeOut()
{
  if(timeout>0)
    timeout--;
}

void resetTimeOut()
{
  timeout=TIMEOUT;
}

void loop()
{
   incrementCount(severalAnalogReads(In));
   incrementTimeOut();
   if(pollButt(P1))
  {
    Serial.println("P1 interrupt!");
    printState();
    if(timeout<=0)
    {
      setMin();
      resetTimeOut();
    }
  }
  if(pollButt(P2))
 {
   Serial.println("P2 interrupt!");
   printState();
   if(timeout<=0)
   {
     setMax();
     resetTimeOut();
   }
 }
  analogWrite(LEDo, displayVal(val)); 
}
