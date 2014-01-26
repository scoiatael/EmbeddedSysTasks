#include "common.h"
#include <sys/stat.h>
#include <fcntl.h>

#define LITTLE_EN 0
#define BIG_EN 1
#define OTHER 2

char test()
{
  char c[4];
  *(int32_t *)c = 0x61626364;
  switch (*c) {
  case 'd':
    return LITTLE_EN;
  case 'a':
    return BIG_EN;
  default:
    return OTHER;
  }
}

//big :: ABCD, lil:DCBA, PDP: BADC

#define toInt16(W) (int16_t*) (W)
void swap2Bytes(char* word)
{
  /*
  char cp[2];
  memcpy(cp,word,2);
  memcpy(word,&word[2],2);
  memcpy(&word[2],cp,2);
  */
  *toInt16(word) = *toInt16(word) ^ *toInt16(&word[2]); 
  *toInt16(&word[2]) = *toInt16(&word[2]) ^ *toInt16(word); 
  *toInt16(word) = *toInt16(word) ^ *toInt16(&word[2]); 
}
void lilToPDP(char* word)
{
  swap2Bytes(word);
}

void PDPToLil(char* word)
{
  swap2Bytes(word);
}

void swapBytes(char* wordHalf)
{
  /*
  char cp = wordHalf[0];
  wordHalf[0] = wordHalf[1];
  wordHalf[1] = cp;
  */
  wordHalf[0] = wordHalf[0] ^ wordHalf[1]; 
  wordHalf[1] = wordHalf[1] ^ wordHalf[0];
  wordHalf[0] = wordHalf[0] ^ wordHalf[1];
}

void bigToPDP(char* word)
{
  swapBytes(word);
  swapBytes(&word[2]);
}

void PDPToBig(char* word)
{
  swapBytes(word);
  swapBytes(&word[2]);
}

int convert(FILE* fin, FILE* fout, void (*conv)(char*))
{
  char buf[4];
  ssize_t ret;
  while( (ret = fread(buf, sizeof(char), 4, fin)) == 4) {
    (*conv)(buf);
    fwrite(buf, sizeof(char), 4, fout);
  }
  if(ret == 0) {
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

int main(int argc, const char *argv[])
{
  FILE *fw, *fr;
  char *c;
  if(argc > 1) {
    c = (char*)malloc(sizeof(char) * (20 + strlen(argv[1])));
    strcpy(c, argv[1]);
  } else {
    c = (char*)malloc(sizeof(char) * 128);
    printf("file to be converted: ");
    scanf("%100s", c);
  }
  CERR((fw = fopen(c, "r")) == NULL, "input file open failed");
  CERR((fr = fopen(strcat(c, "_converted"), "w")) == NULL, "output file open failed");
  printf("convert [t]o or [f]rom PDP?\n ");
  char modeToFrom[64];
  scanf("%64s", modeToFrom);
  int lit = OTHER;
  switch (lit = test()) {
  case LITTLE_EN:
    printf("little endian\n");
    break;
  case BIG_EN:
    printf("big endian\n");
    break;
  case OTHER:
  default:
    printf("unknown\n");
    break;
  }
  if(lit == LITTLE_EN) {
    if(modeToFrom[0] == 'f') {
      return convert(fw,fr,&PDPToLil);
    }
    if(modeToFrom[0] == 't') {
      return convert(fw,fr,&lilToPDP);
    }
  }
  if(lit == BIG_EN) {
    if(modeToFrom[0] == 'f') {
      return convert(fw,fr,&PDPToBig);
    }
    if(modeToFrom[0] == 't') {
      return convert(fw,fr,&bigToPDP);
    }
  }
  return 0;
}
