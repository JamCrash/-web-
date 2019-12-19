
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

int main()
{
  int lisnfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("listen fd = %d\n", lisnfd);
  typedef struct sockaddr sa;
  struct sockaddr_in server;
  bzero(&server, sizeof server);
  inet_pton(lisnfd, "127.0.0.1", &server.sin_addr);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_family = AF_INET;
  server.sin_port = htons(80);
  bind(lisnfd, (sa*)&server, sizeof server);
  listen(lisnfd, 21);
  int infd = accept(lisnfd, NULL, NULL);
  printf("infd = %d\n", infd);
  close(infd);
}
