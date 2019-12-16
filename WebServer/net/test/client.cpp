
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serverAddr;
  bzero(&serverAddr, sizeof serverAddr);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(3000);
  inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);
  connect(fd, (struct sockaddr*)&serverAddr, sizeof serverAddr);
  while(true)
  {
    char buf[20];
    ssize_t n = read(fd, buf, 20);
    write(STDOUT_FILENO, buf, n);
  }
  printf("clien end\n");
  return 0;
}