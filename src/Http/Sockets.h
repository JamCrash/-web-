
#ifndef _SOCKETS_H
#define _SOCKETS_H

#include <strings.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <fcntl.h>

namespace Sockets
{

  int set_nonblock(int fd)
  {
    if(fd < 0) 
      return -1;
    
    int flag;
    if((flag = fcntl(fd, F_GETFL, 0)) < 0)
    {
      perror("get socket file descriptor flag error");
      return -1;
    }
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) < 0)
    {
      perror("set file descriptor error");
      return -1;
    }

    return 0;
  }

  int create_and_bind(uint16_t port, bool setNonBlock)
  {
    typedef struct sockaddr SA;

    int listenFd;
    if((listenFd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("create listen socket failed");
      return listenFd;
    }

    if(setNonBlock)
    {
      if(set_nonblock(listenFd) < 0)
        return -1;
    }

    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(::bind(listenFd, (SA*)&serverAddr, sizeof(serverAddr)) < 0)
    {
      char buf[64];
      inet_ntop(AF_INET, &serverAddr.sin_addr, buf, 64);
      sprintf(buf, "bind socket to %s:%d failed", buf, port);
      perror(buf);
      return -1;
    }

    return listenFd;
  }

  int listen_socket(int fd)
  {
    if(::listen(fd, 128) < 0)
    {
      perror("listen socket error");
      return -1;
    }
    return 0;
  }

  int accept_sockets(int listenFd)
  {
    return ::accept(listenFd, NULL, NULL);
  }

}

#endif  // _SOCKETS_H
