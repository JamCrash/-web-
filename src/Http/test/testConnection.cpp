
#include "../HttpConnection.h"
#include "../../net/EventLoop.h"

#include <sys/socket.h>
#include <strings.h>
#include <stdio.h>
#include <netinet/in.h>

using namespace net;
using namespace Http;

void f(int fd)
{
  EventLoop loop;
  HttpConnection connection(&loop, fd);
  connection.connectionEstablish();
  loop.loop();
}

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serverAddr;
  bzero(&serverAddr, sizeof serverAddr);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(3000);
  serverAddr.sin_family = AF_INET;
  bind(fd, (struct sockaddr*)&serverAddr, sizeof serverAddr);
  listen(fd, 128);
  int infd = accept(fd, NULL, NULL);
  f(infd);
  printf("in main\n");
}