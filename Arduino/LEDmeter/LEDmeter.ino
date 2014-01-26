const int LED=0;

void setup()
{
  Serial.begin(9600);
}

int multipleAnalogRead(int l)
{
  unsigned long sum=0;
  const int COUNT=10;
  for(int i=0; i<COUNT; i++)
    sum+=analogRead(l);
  return sum/COUNT;
}

void loop()
{
  Serial.println(multipleAnalogRead(LED));
  //analogWrite(LED,127);
  delay(500);
}
