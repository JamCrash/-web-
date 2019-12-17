
#include "../EventLoop.h"
#include "../Channel.h"

#include <sys/socket.h>
#include <sys/epoll.h>

using namespace net;

int main()
{
  int epfd=epoll_create1(EPOLL_CLOEXEC);
  
}