// vim:syntax=c:

void setup()
{
  Serial.begin(9600);
}

char cont=0;
unsigned long onesnr = 0;
unsigned long bytenr = 0;
const signed long EDGE=2048;

void writeStats()
{
  Serial.print("Statistics: \n bytes: ");
  Serial.print(bytenr);
  Serial.print(" '1's: ");
  Serial.print(onesnr);
  Serial.print("\n");
}

unsigned int countOnes(unsigned char inByte)
{
  unsigned int sum=0;
  for(int i=0; i<8;i++) {
    sum = sum + (inByte % 2);
    inByte = inByte >> 1;
  }
  return sum;
}

void resetStats()
{
  Serial.print("Resetting counter..\n");
  cont = 0;
  bytenr = 0;
  onesnr = 0;
}

void loop()
{
  if(cont < 0 || bytenr >= EDGE || cont >= EDGE) {
      writeStats();
      resetStats();
  }    
  const int inByte = Serial.read();
  //No input
  if(inByte == -1) {
    delay(100);
  } else {
    //EOL
    if(inByte == '\n') {
      Serial.write("EOL\n");
      cont = -1; 
    }
    //EOF
    if(inByte == '\^') {
      Serial.write("EOF\n");
      cont = -1; 
    }
    if(cont >= 0) {      
      onesnr = onesnr + countOnes(inByte);
      bytenr++;
      Serial.print("input: ");
      Serial.print(int(inByte));
      Serial.print(" (");
      Serial.print(inByte);
      Serial.print(")\n");
      writeStats();
    }
  }
}
