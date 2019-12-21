
#include "../EventLoopThreadPool.h"
#include "../EventLoop.h"

#include <unistd.h>
#include <cassert>

using namespace net;

int main()
{
  EventLoop loop;
  EventLoopThreadPool pool(&loop, 3);
  pool.start();
  sleep(5);
  //assert(pool.getNext() == &loop);
}