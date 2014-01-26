// vim: syntax=c:
#include <stdio.h>

unsigned char reverse(unsigned char r) {
  unsigned char v = 0;
  for(int i=0; i<8;i++) {
    v = v << 1;
    v = v + (r %2);
    r = r >> 1;
  }
  return v;
}
void printAsB(unsigned int e) {
  unsigned int cp = e;
  for(int i=0;i<32;i++) {
    printf("%d ", cp % 2);
    cp = cp >> 1;
  }
  printf("\n");
}

const unsigned int EldestBitMask = 65536UL * 65536UL/2; // 2 ** 32
unsigned int calcCrc(unsigned int value, unsigned int poly) {
  unsigned int checkB = value & EldestBitMask;
  printf("checkB: %u\n", checkB);
  if(checkB > 0) {
    value = value ^ poly;
  }
  return value;
}

unsigned int addBit(unsigned int pattern, unsigned char bit) {
  return (pattern * 2) + (bit % 2);
}

unsigned char getByte() {
  const int MaxDelay = 1000;
  int dl = 100;
  int rd = -1;
  while(rd == -1) {
    if(dl < MaxDelay) {
      dl = 2*dl;
    }
    rd = getchar(); 
  }
  printf(" read %d\n", rd);
  return rd;
}

unsigned int getPattern() {
  unsigned int p=0;
  for(int i=0; i<4; i++) {
    p = p << 8;
    p = p + getByte();
  }
  printf("Read 0x%lx\n", p);
  return p;
}

char validate(unsigned char inByte) {
  if(inByte == '\n') {
    printf("EOL\n");
    return -1;
  }
  if(inByte == '\^') {
    printf("EOF\n");
    return -1;
  }
}

char cont = 0;
unsigned int pattern = 0;
const unsigned int polynomial = 0x04C11DB7;

void resetPattern() {
  cont = 0;
  pattern = getPattern();
  printAsB(pattern);
  pattern = calcCrc(pattern, polynomial);
}

void setup() {
  resetPattern();
}

void writePattern() {
  printf("Calculated pattern: ");
  printf("%u (0x%lx)", pattern, pattern);
  printf(" by polynomial 0x%lx", polynomial); 
  printf("\n: ");
  printAsB(pattern);
}

void loop() {
  if(cont < 0) {
    writePattern();
    resetPattern();
  }
  int inByte = getByte();
  cont = validate(inByte);
  if(cont >=0 ) {
    unsigned int patCp = pattern;
    inByte = reverse(inByte);
    for(int i=0; i<8; i++) {
      printf("New bit: %d\n", inByte % 2);
  //    printAsB(patCp);
      patCp = addBit(patCp, inByte % 2);
      printAsB(patCp);
      printAsB(polynomial);
      patCp = calcCrc(patCp, polynomial);
      printAsB(patCp);
      inByte = inByte >> 1;
      pattern = patCp;
    }  
  }
}
