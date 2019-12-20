
#include "../Sockets.h"
#include "../HttpConnection.h"
#include "../../net/EventLoop.h"

#include <sys/socket.h>
#include <strings.h>
#include <stdio.h>
#include <netinet/in.h>

using namespace net;
using namespace Http;
using namespace Sockets;

EventLoop* g_loop;

void f(int fd)
{
  set_nonblock(fd);
  EventLoop loop;
  g_loop = &loop;
  HttpConnection connection(&loop, fd);
  //connection.setCloseCallBack([])
  connection.connectionEstablish();
  loop.loop();
}

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  printf("fd=%d\n",fd);
  struct sockaddr_in serverAddr;
  bzero(&serverAddr, sizeof serverAddr);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(3000);
  serverAddr.sin_family = AF_INET;
  bind(fd, (struct sockaddr*)&serverAddr, sizeof serverAddr);
  listen(fd, 128);
  int infd = accept(fd, NULL, NULL);

  if(infd > 0)
    f(infd);
  printf("in main\n");
}