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
#include <dirent.h>
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
void send404(int clientSock) {
  char * header = "HTTP/1.1 404 Not Found\n\n";
  send(clientSock,header,strlen(header),0);
  exit(1);
}
void updateHomePage(char * fileName, char * path) {
  char * start = "<!doctype html>\n<html>\n<body><h1>Current Files:</h1>\n";
  char * end = "<form method=\"post\" enctype=\"multipart/form-data\">\n<input name=\"file\" type=\"file\" /><button> Send Request </button>\n</form>\n<p>After uploading, reload the page for changes.</p>\n</body>\n</html>";
  int homePage;
  if (strlen(fileName) == 0) {
    homePage = open("homePage.html",O_WRONLY|O_CREAT|O_TRUNC,0600);
  } else {
    homePage = open(fileName,O_WRONLY|O_CREAT|O_TRUNC,0600);
  }
  write(homePage,start,strlen(start));
  DIR * currentDir;
  currentDir = opendir(path);
  struct dirent * currentFile;
  struct stat stats;
  char * line = malloc(600);
  while (currentFile = readdir(currentDir)) {
    stat(currentFile->d_name,&stats);
    #ifdef _DIRENT_HAVE_D_TYPE
    if (currentFile->d_type == DT_REG) {
      sprintf(line,"<p><a href=\"%s\">%s</a></p>\n", currentFile->d_name, currentFile->d_name);
      //printf("Like: %s\n", line);
      write(homePage,line,strlen(line));
      continue;
    } else if (currentFile->d_type == DT_DIR) {
      sprintf(line,"<p><a href=\"/dir/'%s'\">Directory:%s</a></p>\n", currentFile->d_name, currentFile->d_name);
      write(homePage,line,strlen(line));
      continue;
    }
    #endif
    if (S_ISREG(stats.st_mode)) {
      //printf("Name: %s\n", currentFile->d_name);
      sprintf(line,"<p><a href=\"%s\">%s</a></p>\n", currentFile->d_name, currentFile->d_name);
      //printf("Like: %s\n", line);
      write(homePage,line,strlen(line));
      continue;
    } else if (S_ISDIR(stats.st_mode)){
      sprintf(line,"<p><a href=\"/dir/'%s'\">Directory: %s</a></p>\n", currentFile->d_name, currentFile->d_name);
      write(homePage,line,strlen(line));
    }
  }
  write(homePage,end,strlen(end));
  free(line);
}
void sendFile(int clientSock, char * firstLine) {
  sscanf(firstLine,"GET %s", firstLine);
  int f, bytesToRead, isHTML;
  bytesToRead = 0;
  isHTML = 0;
  struct stat stats;
  if (strlen(firstLine) == 1) {
    updateHomePage("",".");
    f = open("homePage.html", O_RDONLY, 0);
    stat("homePage.html",&stats);
  } else {
    if (strlen(firstLine) >= 6) {
      char * end = (char *) malloc(6);
      strncpy(end, firstLine + strlen(firstLine) - 4, 4);
      if (!strcmp(end,"html")) isHTML = 1;
      //printf("End: %s\n", end);
      free(end);
    }
    if (strlen(firstLine) >= 4 && !strncmp(firstLine,"/dir",4)) {
      printf("Clicked on directory!\n");
      char * path = (char *) malloc(100);
      sscanf(firstLine, "/dir/'%[^']'", path);
      char * fileName = (char *) malloc(125);
      sprintf(fileName,"dirContent %s .html", path);
      printf("File Name: %s\n", fileName);
      printf("Path: %s\n", path);
      updateHomePage(fileName,path);
      f = open(fileName, O_RDONLY, 0);
      stat(fileName,&stats);
      isHTML = 1;
      free(path);
      free(fileName);
    } else {
      f = open(firstLine+1, O_RDONLY, 0);
      if (errno != 0) send404(clientSock);
      stat(firstLine+1,&stats);
    }
  }
  bytesToRead = stats.st_size;
  printf("Bytes: %d\n", bytesToRead);
  void * outFile = malloc(bytesToRead);
  int readAmount = read(f,outFile,bytesToRead);
  char * header = (char *) malloc(256);
  if (strlen(firstLine) == 1 || isHTML) {
    strcpy(header,"HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\n\n");
  } else {
    sprintf(header, "HTTP/1.1 200 OK\nContent-Type: application/octet-stream;\nContent-Disposition: attachment;\nContent-Length: %d\n\n",readAmount);
  }
  void * output = malloc(readAmount+strlen(header)+1);
  memcpy(output,header,strlen(header));
  memcpy(output+strlen(header),outFile,readAmount);
  int amountSent = send(clientSock,output,readAmount+strlen(header),0);
  printf("Amount Sent: %d\n", amountSent);
  free(header);
  free(outFile);
  free(output);
}
void getFile(int clientSock, char * bytesRecieved) {
  char * ptr = bytesRecieved;
  //printf("Got here!\n");
  ptr = bytesRecieved;
  while (1) {
    if (!strncmp(ptr,"boundary=",9)) break;
    ptr++;
  }
  char * line = malloc(256);
  sscanf(ptr,"%[^\n]", line);
  char * boundary = malloc(256);
  sscanf(line, "boundary=%s\n", boundary);
  // printf("Boundary: %s\n", boundary);
  ptr += strlen(boundary);
  while (1) {
    if (!strncmp(ptr,boundary,strlen(boundary))) break;
    ptr++;
  }
  char * startData = ptr;
  startData += strlen(boundary)+2;
  ptr++;
  while (1) {
    if (!strncmp(ptr,boundary,strlen(boundary))) break;
    ptr++;
  }
  char * endData = ptr;
  endData -= 4;
  char * fileName = malloc(256);
  sscanf(startData,"Content-Disposition: form-data; name=\"file\"; filename=\"%[^\"]",fileName);
  //printf("fileName: %s\n", fileName);
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
  //printf("Start: %s\n", startData);
  //printf("End: %s\n", endData);
  int newFile = open(fileName,O_CREAT|O_WRONLY|O_TRUNC,0600);
  //printf("%p, %p\n", endData, startData);
  write(newFile,startData,endData-startData);
  strcpy(line,"HTTP/1.1 202 Accepted\n\nasdf");
  send(clientSock,line,strlen(line),0);
  //printf("Went okay!\n");
  free(fileName);
  free(line);
  free(boundary);
}
void childBehavior(int clientSock) {
  char * request = (char *) malloc(1000000);
  int bytesGot = recv(clientSock,request,1000000,0);
  if (bytesGot == -1) err();
  char * firstLine = (char *) malloc(128);
  sscanf(request,"%[^\n]", firstLine);
  if (!strncmp(firstLine,"GET",3)) {
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
