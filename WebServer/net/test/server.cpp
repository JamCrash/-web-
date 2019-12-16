
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <memory>

#include "../EventLoop.h"
#include "../Channel.h"

using namespace net;

EventLoop* g_loop;
Channel* ch;

void f(int fd)
{
  while(true)
  {
    write(fd, "server data\n", 12);
    sleep(5);
  }
}

int main()
{
  struct sockaddr_in serverAddr;
  bzero(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(3000);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  int listenFd = socket(AF_INET, SOCK_STREAM, 0);
  bind(listenFd, (struct sockaddr*)&serverAddr, sizeof serverAddr);
  listen(listenFd, 128);
  int infd=accept(listenFd, NULL, NULL);
  EventLoop loop;
  g_loop = &loop;
  Channel channel(&loop, infd);
  channel.enableReading();
  channel.setWriteCallBack(std::bind(f, infd));
  channel.enableWriting();
  loop.loop();
  
  printf("in server main\n");
  return 0;
}