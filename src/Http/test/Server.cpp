
#include "../HttpConnection.h"
#include "../../net/EventLoop.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <memory>
#include <functional>

using namespace std;
using namespace net;
using namespace Http;

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  printf("listen fd = %d\n", fd);
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
  HttpConnection conn(&loop, infd);
  //shared_ptr<HttpConnection> connection(new HttpConnection(&loop, infd));
  //connection->setCloseCallBack(bind(&HttpConnection::connectionDestroy, connection));
  //connection->connectionEstablish();
  loop.loop();
}