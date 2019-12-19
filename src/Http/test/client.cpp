
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

int main()
{
  typedef struct sockaddr sa;
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  printf("fd=%d\n",fd);
  struct sockaddr_in client;
  bzero(&client, sizeof client);
  client.sin_family = AF_INET;
  client.sin_port = htons(80);
  inet_pton(AF_INET, "127.0.0.1", &client.sin_addr);
  if(connect(fd, (struct sockaddr*)&client, sizeof(client))<0)
    perror("connection failed");
  close(fd);
}