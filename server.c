#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#define PORT "38203"
void err() {
  printf("Error: %s\n", strerror(errno));
  exit(1);
}
int makeServerSocket() {
  struct addrinfo * serverInfo, * hints;
  hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = AI_PASSIVE;
  if (getaddrinfo(NULL,PORT,hints, &serverInfo)) err();
  int serverSock;
  serverSock = socket(serverInfo->ai_family,serverInfo->ai_socktype,serverInfo->ai_protocol);
  int yes = 1;
  int sockOpt =  setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  bind(serverSock,serverInfo->ai_addr,serverInfo->ai_addrlen);
  char * serverIP = (char *)malloc(128);
  struct sockaddr_in *sockAddr;
  sockAddr = (struct sockaddr_in *)serverInfo->ai_addr;
  inet_ntop(serverInfo->ai_family, &(sockAddr->sin_addr),serverIP, 128);
  printf("Server Address: %s:%s\n", serverIP, PORT);
  listen(serverSock,10);
  freeaddrinfo(hints);
  freeaddrinfo(serverInfo);
  return serverSock;
}
int makeClientSocket(int serverSock) {
  struct sockaddr_storage clientAddr;
  int clientSock;
  int size = sizeof(struct sockaddr_storage);
  clientSock = accept(serverSock,(struct sockaddr *)&clientAddr,&size);
  printf("Connection Success!\n");
  return clientSock;
}
int main() {
  int serverSock = makeServerSocket();
  while (1) {
      int clientSock = makeClientSocket(serverSock);
      char * request = (char *) malloc(1024);
      int bytesGot = recv(clientSock,request,1024,0);
      if (bytesGot == -1) err();
      printf("Recieved data: \n%s, numBytes: %d\n", request, bytesGot);
      char * firstLine = (char *) malloc(128);
      sscanf(request,"%[^\n]", firstLine);
      if (!strncmp(request,"GET",3)) {
        //TODO: Read the request, collect the file, and send it to the client.
        void * outFile = malloc(1024);
        int f = open("test.html", O_RDONLY, 0);
        read(f,outFile,1024);
        void * output = malloc(1100);
        char * header = "HTTP/1.1 200 OK\n";
        memcpy(output,header,strlen(header));
        memcpy(output+strlen(header),outFile,1024);
        send(clientSock,outFile,1024,0);
      }
      close(clientSock);
  }
}
