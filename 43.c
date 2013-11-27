#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>


unsigned long long norm_fib(unsigned long long arg)
{
  if(arg < 2) {
    return 1;
  }
  return norm_fib(arg-1)+norm_fib(arg-2);
}
/*
typedef key_t unsigned long long;
typedef val_t unsigned long long;
struct tree
{
  key_t key;
  val_t val;
  struct tree* Left;
  struct tree* Right;
} T*;

val_t* find(struct tree* t, key_t arg)
{
  if(t==NULL)
    return NULL;
  if(t->key == arg)
  {
    return &t->val;
  }
  if(arg < t->key)
  {
    return find(t->left, arg);
  }
  return find(t->right,arg);
}

struct tree* insert(struct tree* t, key_t k, val_t v)
{
  if(t==NULL) {
    struct tree* tp = (struct tree*) malloc(sizeof(struct tree));
    tp->val = v;
    tp->key = k;
    return tp;
  }
  if(k < t->key) {
    t->left=insert(t->left,k,v);
  } else {
    t->right=insert(t->right,k,v);
  }
  return t;
}
*/
#define MAX_FARG 90
unsigned long long *tab=NULL;

unsigned long long _fib(unsigned long long arg)
{
  assert(tab!=NULL);
  register unsigned long long p1=1;
  tab[0]=1;
  for(register unsigned long long i=1,p2=1,cp;i<arg;i++)
  {
    assert(p1 < (ULLONG_MAX>>1));
    tab[i]=p1;
    cp=p2;
    p2=p1;
    p1=cp+p1;
  }
  return p1;
}

unsigned long long fib(unsigned long long arg)
{
  assert(arg < MAX_FARG);
  if(tab==NULL) {
    tab = (unsigned long long*)malloc(sizeof(unsigned long long) * (MAX_FARG+1));
    _fib(MAX_FARG);
  }
  return tab[arg];
}

int main(int argc, const char *argv[])
{
  unsigned long long value=0;
  int cont = 0 > 1;
  for(int i=1; i<argc;i++)
  {
    if(strcmp(argv[i],"-i")==0) {
      cont = 1 > 0;
      break;
    }
    value = atoll(argv[i]);
    printf("x: %lld\n", value);
    printf("fib_x: %llu\n", fib(value));
  }
  while(cont) {
    printf("x: ");
    scanf("%llu", &value);
    printf("fib_x: %llu\n", fib(value));
  }
  return 0;
}
