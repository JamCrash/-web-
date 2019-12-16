
#include "../EventLoop.h"

#include <sys/epoll.h>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <pthread.h>

using namespace net;

EventLoop* g_loop;

void* f(void* arg)
{
  sleep(1);
  g_loop->quit();
  return NULL;
}

int main()
{
  /*
  EventLoop loop;
  EventLoop loop2;
  */
 
  EventLoop loop;
  pthread_t tid;
  g_loop=&loop;
  pthread_create(&tid, NULL, f, NULL);
  loop.loop();
  pthread_join(tid, NULL);
  printf("in main thread\n");
}
