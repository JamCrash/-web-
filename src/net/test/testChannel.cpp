
#include "../EventLoop.h"
#include "../Channel.h"

#include <sys/epoll.h>
#include <stdio.h>
#include <cassert>

using namespace net;

int main()
{
  EventLoop loop;
  Channel channel(&loop, 1);
  assert(channel.getStatus() == Channel::cDisconnected_);
  channel.enableReading();
  assert(!(channel.isNonEvent()));
  assert(channel.getStatus() == Channel::cConnected_);
  assert((channel.getEvents()) & EPOLLIN);
  channel.enableWriting();
  assert(!(channel.isNonEvent()));
  assert((channel.getEvents()) & EPOLLOUT);
  printf("************Channel test ok*****************\n");
}