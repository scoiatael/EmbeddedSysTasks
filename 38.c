#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//mem opti
void memMoveSpaces(char* tab, int size)
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

int main(int argc, const char *argv[])
{
  for(int i=1; i<argc; i++) {
    puts(argv[i]);
    int size = strlen(argv[i]);
    char copy[size];
    strcpy(copy, argv[i]);
    memMoveSpaces(copy, size);
    puts(copy);
  } 
  return 0;
}
