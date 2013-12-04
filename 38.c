#include "common.h"

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

void betMoveSpaces(char* tab, int size)
{
  int i,count=0,tabp[size];
  for (i = 0; i < size; i++) {
    if(tab[i]==' ') { 
      tab[count]=' ';
      count++; 
    } else {
      tabp[i-count]=tab[i];
    }
  }
  for(i = 0; i < size-count; i++) {
    tab[i+count] = tabp[i]; 
  }
}

void forMoveSpaces(char* tab, int size)
{
  int spaceCount=0;
  for (int i = 0; i < size; i++) {
    if(tab[i]==' '){
      spaceCount++;
    } else {
      tab[i-spaceCount] = tab[i];
    }
  }
  for (int i = 0; i < spaceCount; i++) {
    tab[size-spaceCount-1+i] = ' ';
  }
}

void mMoveSpaces(char* tab, int size)
{
  int i,count=0,tabp[size];
  for (i = 0; i < size; i++) {
    if(tab[i]==' ') { 
      tab[count]=' ';
      count++; 
    } else {
      tabp[i-count]=tab[i];
    }
  }
  for(i = 0; i < size-count; i++) {
    tab[i+count] = tabp[i]; 
  }
}


#define COUNT 8000000
int main(int argc, const char *argv[])
{
  for(int i=1; i<argc; i++) {
    int size;
    if(strcmp(argv[i],"-i")==0)
    {
      size = COUNT+400;
    } else {
      size = strlen(argv[i]);
    }
    char copy[size],copy1[size],copy2[size];
    if(strcmp(argv[i],"-i")==0)
    {
      //scanf("%1000000[^\n]", copy);
      fread(copy, 1, COUNT, stdin);
    } else {
      strcpy(copy, argv[i]);
    }
    size = strlen(copy);
    puts(copy);
    fprintf(stderr, "size: %d\n", size);
    puts("---");
    strcpy(copy1, copy);
    strcpy(copy2, copy);
    struct timeval t1,t2;
    DIFF(moveSpaces(copy1,size));
    puts(copy1);
    fprintf(stderr, "backwards: %ld\n", timediff(&t1,&t2));
    DIFF(betMoveSpaces(copy, size));
    puts(copy);
    fprintf(stderr, "forwards: %ld\n", timediff(&t1,&t2));
    DIFF(forMoveSpaces(copy2, size));
    puts(copy2);
    fprintf(stderr, "forwards2: %ld\n", timediff(&t1,&t2));
  } 
  return 0;
}
