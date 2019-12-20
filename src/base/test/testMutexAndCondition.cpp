
#include "../Mutex.h"
#include "../Condition.h"

#include <cstdio>

using namespace Base;

MutexLock mutex;
Condition cond(mutex);
int i=1;
void* f(void*)
{
  MutexLockGuard guard(mutex);
  while(i<=50)
  {
    while(i%2==1)
      cond.wait();
    if(i<=50)
      printf("thread: %d\n", i++);
    cond.signal();
  }
  printf("thread end\n");
  return NULL;
}

int main()
{
  pthread_t tid;
  pthread_create(&tid, NULL, f, NULL);
  MutexLockGuard guard(mutex);
  while(i<=50)
  {
    while(i%2==0)
      cond.wait();
    if(i<=50)
      printf("main: %d\n",i++);
    cond.signal();
  }
  printf("main end\n");
  pthread_join(tid, NULL);
  printf("test mutex and condition variable successfully\n");
  return 0;
}
