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
#include <sys/stat.h>
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
  free(serverIP);
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
int send404(int clientSock) {
  char * header = "HTTP/1.1 404 Not Found\n\n";
  send(clientSock,header,strlen(header),0);
  exit(1);
}
int sendFile(int clientSock, char * firstLine) {
  sscanf(firstLine,"GET %s", firstLine);
  int f, bytesToRead;
  struct stat stats;
  if (strlen(firstLine) == 1) {
    f = open("test.html", O_RDONLY, 0);
    stat("test.html",&stats);
  } else {
    f = open(firstLine+1, O_RDONLY, 0);
    if (errno != 0) send404(clientSock);
    stat(firstLine+1,&stats);
  }
  bytesToRead = stats.st_size;
  void * outFile = malloc(bytesToRead+1);
  int readAmount = read(f,outFile,bytesToRead);
  char * header;
  if (strlen(firstLine) == 1) {
    header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\n\n";
  } else {
    header = (char *) malloc(256);
    sprintf(header, "HTTP/1.1 200 OK\nContent-Type: application/octet-stream;\nContent-Disposition: attachment;\nContent-Length: %d\n\n",readAmount);
  }
  void * output = malloc(readAmount+strlen(header)+1);
  memcpy(output,header,strlen(header));
  memcpy(output+strlen(header),outFile,readAmount);
  int amountSent = send(clientSock,output,readAmount+strlen(header)+1,0);
  printf("Amount Sent: %d\n", amountSent);
  if (strlen(firstLine) != 1) free(header);
  free(outFile);
  free(output);
}
int getFile(int clientSock, char * bytesRecieved) {
  char * ptr = bytesRecieved;
  printf("Got here!\n");
  while (1) {
    if (!strncmp(ptr,"Content-Length",14)) break;
    ptr++;
  }
  char * line = malloc(256);
  sscanf(ptr,"%[^\n]", line);
  int fileSize = 0;
  sscanf(line, "Content-Length: %d\n", &fileSize);
  printf("File Size: %d\n", fileSize);
  ptr = bytesRecieved;
  while (1) {
    if (!strncmp(ptr,"boundary=",9)) break;
    ptr++;
  }
  sscanf(ptr,"%[^\n]", line);
  char * boundary = malloc(256);
  sscanf(line, "boundary=%s\n", boundary);
  //printf("Boundary: %s\n", boundary);
  ptr += strlen(boundary);
  while (1) {
    if (!strncmp(ptr,boundary,strlen(boundary))) break;
    ptr++;
  }
  char * startData = ptr;
  startData += strlen(boundary)+2;
  while (1) {
    if (!strncmp(ptr,boundary,strlen(boundary))) break;
    ptr++;
  }
  char * fileName = malloc(256);
  sscanf(startData,"Content-Disposition: form-data; name=\"file\"; filename=\"%s\"",fileName);
  int first = 0;
  while (1) {
    if (!strncmp(startData,"\n",1)) {
      //printf("TRUE\n");
      if (first == 2) break;
      first++;
    }
    startData++;
  }
  startData++;
  printf("Data Block: %s\n", startData);
  fileName[strlen(fileName)-1] = NULL;
  int newFile = open(fileName,O_CREAT|O_WRONLY,0600);
  write(newFile,startData,fileSize);
  free(fileName);
  free(line);
  free(boundary);
}
int childBehavior(int clientSock) {
  char * request = (char *) malloc(1000000);
  int bytesGot = recv(clientSock,request,1000000,0);
  if (bytesGot == -1) err();
  char * firstLine = (char *) malloc(128);
  sscanf(request,"%[^\n]", firstLine);
  if (!strncmp(firstLine,"GET",3)) {
    //TODO: Read the request, collect the file, and send it to the client.
    sendFile(clientSock,firstLine);
  }
  if (!strncmp(firstLine,"POST",4)) {
    getFile(clientSock,request);
  }
  free(request);
  free(firstLine);
}
int main() {
  int serverSock = makeServerSocket();
  while (1) {
    int clientSock = makeClientSocket(serverSock);
      pid_t p;
      p = fork();
      if (p == 0) {
        childBehavior(clientSock);
      } else {
        close(clientSock);
      }
  }
}
