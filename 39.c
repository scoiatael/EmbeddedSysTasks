#include "common.h"

#define memLimitExp 18
#define memLimit (1<<memLimitExp)
#define memLimitGrainExp 2
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
    return NULL;
  } 
  a->cur = a->cur + amount;
  return &a->mem[a->cur-amount];
}

void cpuAllocFree(struct cpuAlloc* a __attribute__((unused)), char* p __attribute__((unused)))
{
  return;
}

void cpuAllocDebug(struct cpuAlloc* a)
{
  printf("cur: %d\n", a->cur);
}

//------------------------------

#define freeMask 0b10000000
#define rowMask  0b01111111

#define MTRUE 0b11111111
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
  s->base = (s->base & rowMask) | (f & freeMask);
}

char rowNr(struct _memAllocS s)
{
  return s.base & rowMask;
}

void setRowNr(struct _memAllocS* s, char f)
{
  s->base = (s->base & freeMask) | (f & rowMask);
}

void printThis(struct _memAllocS s)
{
  printf("[F: %s R: %d]", (isFree(s)) ? "TRUE" : "FALSE", rowNr(s)); 
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
  setRowNr(a->map, memAllocSExp);
  setFree(a->map, MTRUE);
  printf("Max chunk size: %d(%d)\n", memAllocSNr, memAllocSExp);
  putchar('\n');
}

#define LOG2(X,B) ((B)-__builtin_clz(X))
#define LOG2aT(X) LOG2(X,sizeof(allocT)*8)

/* returns address (number) of first block allocated -> contingous allocation */
allocT _memAllocAlloc(struct memAlloc* a, allocT i /* log of number of blocks (of memLimitGrain size) to allocate */)
{
  printf("Allocating %d(%d) blocks..", 1<<(i), i);
  for(allocT j=0; j < memAllocSNr; j+=(1<<i))
  {
    if(rowNr(a->map[j])==i && isFree(a->map[j]))
    {
      printf("found space at %d\n", j);
      setFree(&a->map[j],MFALSE);
      return j;
    }
  } 
  if((i+1) > memAllocSExp) {
    return (allocT)-1;
  }
  allocT j = _memAllocAlloc(a, i + 1);
  if(j == -1) {
    return (allocT)-1;
  }
  setFree(&a->map[j+(1<<i)], MTRUE);
  setRowNr(&a->map[j], i);
  setRowNr(&a->map[j+(1<<i)], i);  
  return j;
}
//---------------------------------
//wersja iteracyjna - todo
allocT _memAllocAllocI(struct memAlloc* a __attribute__((unused)), allocT i __attribute__((unused)))
{
  return -1;
}

// returns address of first byte allocated
char* memAllocAlloc(struct memAlloc* a, allocT from /* number of bytes to allocate */)
{
  from = from / memLimitGrain;
  from = LOG2aT(from);
  allocT ret = _memAllocAlloc(a, from );
  if(ret == -1) {
    return NULL;
  }
  return &a->mem[ret*memLimitGrain];
}

// wersja rekurencyjna
//
void _memAllocFree(struct memAlloc* a, allocT j /* address of first allocated block */)
{
  // assert( this block was already marked free, all remains is to fix row nrs)
  char r = rowNr(a->map[j]);
  printf(" free at row %d.. ",r);
  if(j % r == 0) {
    if(r < memAllocSExp && r == rowNr(a->map[j+(1<<r)]) && isFree(a->map[j+(1<<r)]) ) {
      setRowNr(&a->map[j+(1<<r)], r+1);
      setRowNr(&a->map[j], r+1);
      _memAllocFree(a, j);
    }
  } else {
    if(r < memAllocSExp && r == rowNr(a->map[j-(1<<r)]) && isFree(a->map[j-(1<<r)]) ) {
      setRowNr(&a->map[j-(1<<r)], r+1);
      setRowNr(&a->map[j], r+1);
      _memAllocFree(a, j-r);
    } 
  }
  putchar('\n');
}

// wersja iteracyjna
// TODO
void _memAllocFreeI(struct memAlloc* a __attribute__((unused)), allocT j __attribute__((unused)))
{
}

void memAllocFree(struct memAlloc* a, char* p)
{
  allocT j = (p - a->mem) / memLimitGrain;
  setFree(&a->map[j], MTRUE);
  _memAllocFree(a, j);
}

void memAllocDebug(struct memAlloc* a)
{
  assert(a != NULL);
  for(int i=0; i < memAllocSNr; i = i + (1 << rowNr(a->map[i])))
  {
    printThis(a->map[i]);
    putchar('\n');
  }
}

int main(int argc, const char *argv[])
{
  struct cpuAlloc c;
  cpuAllocInit(&c);
  struct memAlloc m;
  memAllocInit(&m);
  if(argc == 1 || strcmp(argv[1],"-i")==0) {
    unsigned long long num;
    for(int try=0;;try++) {
      printf("[a]lloc, [f]ree or [d]ebug?\n");
      char ch = ' ';
      while(ch == ' ' || ch == '\n') {
        scanf("%c", &ch);
      }
      switch(ch) {
        case 'a':
          printf("amount: ");
          scanf("%lld", &num);
          char* mem;
          if(NULL == (mem = cpuAllocAlloc(&c, num)) ) {
            printf("cpuAlloc Error\n");
          } else {
            memset(mem, (unsigned char)try, num);
            printf(" cpuAlloc pointer: %p\n", mem);
          }
          if(NULL == (mem = memAllocAlloc(&m, num)) ) {
            printf("memAlloc Error\n");
          } else {
            memset(mem, (unsigned char)try, num);
            printf(" memAlloc pointer: %p\n", mem);
          }
          break;
        case 'f':
          printf("pointer for cpuAlloc: ");
          scanf("%llx", &num);
          cpuAllocFree(&c, (char*)num);
          printf("pointer for memAlloc: ");
          scanf("%llx", &num);
          memAllocFree(&m, (char*)num);
          break;
        case 'd':
          cpuAllocDebug(&c);
          memAllocDebug(&m);
          break;
        default:
          printf("only [a], [f] or [d], please\n");
      }
    }
  } else {
    /* ..TODO.. */
  }
  return 0;
}
