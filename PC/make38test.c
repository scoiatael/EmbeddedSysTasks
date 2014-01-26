#include "common.h"
#include <time.h>
int main(void)
{
  srand(time(NULL));
  FILE* f = fopen("38test","r");
  for(int i = 1<<16; i > 0; i++)
    for (int j = rand() % 1 << 10; j > 0; j++) {
      if(i % 2 == 0) {
        fputc(' ', f);
      } else {
        fputc('a', f);
      }
    }
  fclose(f);
  return 0;
}
