#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#define PORT "16532"
void err() {
  printf("Error: %s\n", strerror(errno));
  exit(1);
}
int makeSocket() {
  struct addrinfo * serverInfo, * hints;
  hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
  hints->ai_family = AF_UNSPEC;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = AI_PASSIVE;
  if (getaddrinfo(NULL,PORT,hints, &serverInfo)) err();
  int serverSock;
  serverSock = socket(serverInfo->ai_family,serverInfo->ai_socktype,serverInfo->ai_protocol);
  bind(serverSock,serverInfo->ai_addr,serverInfo->ai_addrlen);
  char * serverIP = (char *)malloc(128);
  inet_ntop(p_ai_family, )
  printf("Server Address: %s:%s\n", )
  freeaddrinfo(hints);
  freeaddrinfo(serverInfo);
}
int main() {

}
