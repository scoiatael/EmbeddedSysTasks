// vim: syntax=c:

void printAsB(unsigned long e) {
  unsigned long cp = e;
  for(int i=0; i< 32; i++) {
    Serial.print(int(cp % 2));
    cp = cp >> 1;
  }
  Serial.print("\n");
}

const long EldestBitMask = 65536UL * 65536UL/2; // 2 ** 31
unsigned long calcCrc(unsigned long value, unsigned long poly) {
  if((value & EldestBitMask) != 0) {
    value = value ^ poly;
  }
  return value;
}

unsigned long addBit(unsigned long pattern, unsigned char bit) {
  return (pattern << 1) + (bit % 2);
}

unsigned long getPattern() {
  unsigned long p=0;
  for(int i=0; i<4; i++) {
    p = p << 8;
    p = p + getByte();
  }
  return p;
}

unsigned char getByte() {
  const int MaxDelay = 1000;
  int dl = 100;
  int rd = -1;
  while(rd == -1) {
    delay(dl);
    if(dl < MaxDelay) {
      dl = 2*dl;
    }
    rd = Serial.read(); 
  }
  Serial.print(" read ");
  Serial.print(rd);
  Serial.print("\n");
  return rd;
}

char validate(unsigned char inByte) {
  if(inByte == '\n') {
    Serial.print("EOL\n");
    return -1;
  }
  if(inByte == '\^') {
    Serial.print("EOF\n");
    return -1;
  }
}

void setup() {
  Serial.begin(115200);
  resetPattern();
}

char cont = 0;
unsigned long pattern = 0;
const unsigned long polynomial = 0x04C11DB7;

void resetPattern() {
  cont = 0;
  pattern = getPattern();
}

void writePattern() {
  Serial.print("Calculated crc32: ");
  Serial.print(pattern);
  Serial.print("\n");
}

void loop() {
  if(cont < 0) {
    writePattern();
    resetPattern();
  }
  int inByte = getByte();
  cont = validate(inByte);
  if(cont >=0 ) {
    unsigned long patCp = pattern;
    for(int i=0; i<8; i++) {
      patCp = calcCrc(addBit(patCp, inByte % 2), polynomial);
      printAsB(pattern); printAsB(polynomial); printAsB(patCp);
      inByte = inByte >> 1;
      pattern = patCp;
    }  
  }
}
