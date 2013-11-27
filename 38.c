#include "common.h"

//mem opti
void optMoveSpaces(char* tab, int size)
{
  int spaceCount=0,nonSpaceStreak=0;
  for (int i = size-1; i>=0; i=i-1) {
    if(tab[i]==' '){
      spaceCount++;
      memcpy(&tab[i+spaceCount], &tab[i+1], nonSpaceStreak);
      nonSpaceStreak=0;
    } else {
      nonSpaceStreak++;
    }
  }
  memcpy(&tab[spaceCount], tab, nonSpaceStreak);
  memset(tab,' ',spaceCount);
}

void moveSpaces(char* tab, int size)
{
  int spaceCount=0;
  for (int i = size-1; i>=0; i=i-1) {
    if(tab[i]==' '){
      spaceCount++;
    } else {
      tab[i+spaceCount] = tab[i];
    }
  }
  for (int i = 0; i < spaceCount; i++) {
    tab[i] = ' ';
  }
}

int main(int argc, const char *argv[])
{
  for(int i=1; i<argc; i++) {
    int size;
    if(strcmp(argv[i],"-i")==0)
    {
      size = 1024;
    } else {
      size = strlen(argv[i]);
    }
    char copy[size],copy1[size];
    if(strcmp(argv[i],"-i")==0)
    {
      scanf("%1000[^\n]", copy);
    } else {
      strcpy(copy, argv[i]);
    }
    puts(copy);
    puts("---");
    strcpy(copy1, copy);
    struct timeval t1,t2;
    DIFF(moveSpaces(copy1,size));
    puts(copy1);
    fprintf(stderr, "without opti: %ld\n", timediff(&t1,&t2));
    DIFF(optMoveSpaces(copy, size));
    puts(copy);
    fprintf(stderr, "with opti: %ld\n", timediff(&t1,&t2));
  } 
  return 0;
}
