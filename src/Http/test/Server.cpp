
#include "../HttpConnection.h"
#include "../../net/EventLoop.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

using namespace net;
using namespace Http;

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serverAddr;
  bzero(&serverAddr, sizeof serverAddr);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(80);
  bind(fd, (struct sockaddr*)&serverAddr, sizeof serverAddr);
  listen(fd, 128);
  int infd = accept(fd, NULL, NULL);
  printf("infd=%d\n", infd);
  EventLoop loop;
  HttpConnection connection(&loop, infd);
  connection.setCloseCallBack([&connection]{ connection.connectionDestroy(); });
  connection.connectionEstablish();
  loop.loop();
}