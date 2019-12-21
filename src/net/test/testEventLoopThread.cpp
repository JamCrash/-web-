
#include "../EventLoopThread.h"
#include "../../base/Thread.h"
#include "../EventLoop.h"

#include <unistd.h>
#include <stdio.h>

using namespace net;

int main()
{
  EventLoopThread loopThread;
  EventLoop* loop = loopThread.startLoop();
  loop->quit();
  printf("in main\n");
  return 0;
}
