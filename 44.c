#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


typedef pixel unsigned char;

#define H 1000
#define W 1000

pixel Screen[W][H];

unsigned long long been[W/64][H/64];

void reset
void redrawScreen()
{
  srand();
  for (int i = 0; i < W; i++) {
    for (int j = 0; j < H; j++) {
      Screen[i][j] = rand() % 256;
    }
  }
}
