#include "common.h"

int no_opti(int height)
{
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < j; i++) {
      putchar('*');
    }
    putchar('\n');
  }
  return EXIT_SUCCESS;
}

//#define mem_opti(H){for (int j = 0; j < (H); j++) {for (int i = 0; i < j; i++) {putchar('*');}putchar('\n');}return EXIT_SUCCESS;}
//

int mem_opti(int height)
{
  for (register int j = 0; j < height; j++) {
    for (register int i = 0; i < j; i++) {
      putchar('*');
    }
    putchar('\n');
  }
  return EXIT_SUCCESS;
}

#define BORDER 1000

int cpu_opti(int height)
{
  if(height < BORDER) {
    char buf[(height + 1) * (height + 1) + 1];
    memset(buf, '*', sizeof(buf));
    int offset = 0;
    for (int i = 0; i < height; i++) {
      buf[offset] = '\n';
      offset += i + 2;
    }
    buf[offset - height - 2 + 1] = '\0';
    puts(buf);
    return EXIT_SUCCESS;
  } else {
    for (int i = 0; i < height; i++) {
      char buf[i+2];
      memset(buf, '*', sizeof(buf));
      buf[i] = '\n';
      buf[i] = '\0';
      puts(buf);
    }
    return EXIT_SUCCESS;
  }
}

void desc(const char *name )
{
  char buf[64];
  sprintf(buf, "Usage: %s { mem | cpu } <height> : options enable specific optimizations\n", name);
  fputs(buf, stderr);
  exit(EXIT_FAILURE);
}


int main(int argc, const char *argv[])
{
  if(argc < 2 || strcmp(argv[1], "help") == 0) {
    desc(argv[0]);
  }
  
  struct timeval t1, t2;  
  DIFF(no_opti(atoi(argv[1])));
  printf("without opt: %ld\n", timediff(&t1,&t2));
  DIFF(cpu_opti(atoi(argv[1])));
  printf("with opt: %ld\n", timediff(&t1,&t2));
  return 0;
}
