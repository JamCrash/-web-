
#include <stdio.h>
#include <arpa/inet.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  typedef struct sockaddr SA;
  struct sockaddr_in serverAddr;
  bzero(&serverAddr, sizeof serverAddr);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(3000);
  inet_pton(fd, "127.0.0.1", &serverAddr.sin_addr);
  connect(fd, (SA*)&serverAddr, sizeof serverAddr);
  int i=0;
  while(true)
  {
    char buf[200] = "afasq\n";
    write(fd, buf, 6);
    i++;
    printf("i=%d\n", i);
    //sleep(5);
    break;
  }
  char buf[222];
  int n = read(fd, buf, 222);
  write(STDIN_FILENO, buf, n);
  printf("in main\n");
}