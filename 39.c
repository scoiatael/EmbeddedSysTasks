#include "common.h"

#define memLimitExp 20
#define memLimit (1<<memLimitExp)
#define memLimitGrainExp 5
#define memLimitGrain (1<<memLimitGrainExp)

typedef int allocT;
struct cpuAlloc 
{
  char mem[memLimit];
  allocT cur;
};

void cpuAllocInit(struct cpuAlloc* a)
{
  a->cur = 0;
}

char* cpuAllocAlloc(struct cpuAlloc* a, allocT amount)
{
  if(a->cur + amount >= memLimit) {
    return (char*)-1;
  } 
  a->cur = a->cur + amount;
  return &a->mem[a->cur-amount];
}

void cpuAllocFree(struct cpuAlloc* a __attribute__((unused)), char* p __attribute__((unused)))
{
  return;
}

//------------------------------

#define freeMask 0b10000000
#define rowMask 0b01111111

#define MTRUE 1
#define MFALSE 0

struct _memAllocS
{
  char base;
};

char isFree(struct _memAllocS s)
{
  return s.base & freeMask;
}

void setFree(struct _memAllocS* s, char f)
{
  s->base = s->base | (f & freeMask);
}

char rowNr(struct _memAllocS s)
{
  return s.base & rowMask;
}

void setRowNr(struct _memAllocS* s, char f)
{
  s->base = s->base | (f & rowMask);
}

#define memAllocSExp (memLimitExp - memLimitGrainExp)
#define memAllocSNr (1<< memAllocSExp)
struct memAlloc
{
  char mem[memLimit];
  struct _memAllocS map[memAllocSNr];
};

void memAllocInit(struct memAlloc* a)
{
  assert(a!=NULL);
  memset(a->map, 0, sizeof(a->map));
  setFree(a->map, MFALSE);
  setRowNr(a->map, memLimitExp);
}

#define LOG2(X,B) (B)-__builtin_clz(X)
#define LOG2aT(X) LOG2(X,sizeof(allocT)*8)

allocT _memAllocAlloc(struct memAlloc* a, allocT i)
{
  i = LOG2aT(i);
  for(allocT j=0; j < memAllocSNr; j+=i)
  {
    if(rowNr(a->map[j])==i && isFree(a->map[j]))
    {
      setFree(&a->map[j],MFALSE);
      return j;
    }
  } 
  if((i<<1) > memLimitExp) {
    return (allocT)-1;
  }
  allocT j = _memAllocAlloc(a, i<<1);
  if(j == -1) {
    return (allocT)-1;
  }
  setFree(&a->map[j+i], MTRUE);
  setRowNr(&a->map[j], i);
  setRowNr(&a->map[j+i], i);  
  return j;
}

char* memAllocAlloc(struct memAlloc* a, allocT from)
{
  allocT ret = _memAllocAlloc(a, from);
  if(ret == -1) {
    return (char*)-1;
  }
  return &a->mem[ret];
}

void _memAllocFree(struct memAlloc* a, allocT j)
{
  char r = rowNr(a->map[j]);
  if(j % r == 0) {
    if(r == rowNr(a->map[j+r]) && isFree(a->map[j+r]) && r < memAllocSExp) {
      setRowNr(&a->map[j+1], r+1);
      _memAllocFree(a, j);
    }
  } else {
    if(r == rowNr(a->map[j-r]) && isFree(a->map[j-r]) && r < memAllocSExp) {
      setRowNr(&a->map[j+1], r+1);
      _memAllocFree(a, j-r);
    } 
  }
}

void memAllocFree(struct memAlloc* a, char* p)
{
  allocT j = (p - a->mem) >> (sizeof(char)-1);
  setFree(&a->map[j], MTRUE);
  _memAllocFree(a, j);
}

int main(void)
{
  printf("nope\n");
  return 0;
}
