#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

#define mem_opti(height) {for(int j=0; j<(height+1)*(height+2)/2; j++) { putchar('*'); if(j) {  putchar('\n'); } } return EXIT_SUCCESS;}

int cpu_opti(int height)
{
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
  if(strcmp(argv[1], "mem") == 0) {
    if(argc < 3) {
      desc(argv[0]);
    }
    return mem_opti(atoi(argv[2]));
  } else if(strcmp(argv[1], "cpu") == 0) {
    if(argc < 3) {
      desc(argv[0]);
    }
    return cpu_opti(atoi(argv[2]));
  } else {
    printf("Running without optimizations..\n");
    return no_opti(atoi(argv[1]));
  }
  return 0;
}
