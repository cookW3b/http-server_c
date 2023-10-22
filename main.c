#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  char buffer[BUFFER_SIZE];
  char response[] = "HTTP/1.0 200OK\r\n"
                    "Content-type: text/html\r\n\r\n"
                    "<html> Simple http web server in C</html>";

  int socketFd = socket(AF_INET, SOCK_STREAM, 0);

  if (socketFd == -1) {
    perror("[http-server] Cannot create socket");
    return 1;
  }

  printf("[http-server] Socket created\n");

  struct sockaddr_in hostAddr;

  int hostAddrLen = sizeof(hostAddr);

  hostAddr.sin_family = AF_INET;
  hostAddr.sin_port = htons(PORT);
  hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  int bindResult = bind(socketFd, (struct sockaddr*)&hostAddr, hostAddrLen);

  if (bindResult == -1) {
    perror("[http-server] Cannot bind socket");
  }

  printf("[http-server] Socket binded\n");

  int listenResult = listen(socketFd, SOMAXCONN);

  if (listenResult == -1) {
    perror("[http-server] Cannot listen incoming connection");
  }

  printf("[http-server] Listening incoming connection\n");

  struct sockaddr_in clientAddr;
  int clientAddrLen = sizeof(clientAddr);

  for (;;) {
    int newSocketFd = accept(socketFd, (struct sockaddr*)&hostAddr, (socklen_t*)&hostAddrLen);

    if (newSocketFd == -1) {
      perror("[http-server] Cannot accept incoming connection");
      continue;
    }

    int socketName = getsockname(newSocketFd, (struct sockaddr*)&clientAddr, (socklen_t*)&clientAddrLen);

    if (socketName == -1) {
      perror("[http-server] Cannot get socket name");
      continue;
    }

    int readSocket = read(newSocketFd, buffer, BUFFER_SIZE);

    if (readSocket == -1) {
      perror("[http-server] Cannot read");
      continue;
    }

    char method[BUFFER_SIZE], version[BUFFER_SIZE], uri[BUFFER_SIZE];

    sscanf(buffer, "%s %s %s", method, uri, version);
    printf("[http-server] Connection accepted by %s:%u\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    printf("[http-server] %s | %s | %s\n\n", version, method, uri);

    int writeSocket = write(newSocketFd, response, strlen(response));

    if (writeSocket == -1) {
      perror("[http-server] Cannot write");
      continue;
    }

    close(newSocketFd);
  }

  return 0;
}
