
#include "../Thread.h"

#include <unistd.h>
#include <cassert>
#include <stdio.h>

using namespace Base;
using namespace CurrentThread;

struct Test
{
  Test()
  {
    assert(isMainThread());
    printf("before main: in main thread\n");
  }
};

Test t;

void* f(void* arg)
{
  assert(!isMainThread());
  printf("in another thread\n");
}

int main()
{
  pthread_t tid;
  pthread_create(&tid, NULL, f, NULL);
  sleep(1);
  assert(isMainThread());
  printf("in main thread\n");
  printf("***************** CurrentThread module test success *************\n");
}