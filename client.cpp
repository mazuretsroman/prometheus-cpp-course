#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int handleStep(int res) {
  if (res == -1) {
    std::cout << "Error: " << std::strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }
  return res;
}

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3001);
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
  handleStep( connect(fd, (struct sockaddr *) &addr, sizeof(addr)) );
  write(fd, "Hello\n", 6);
  char buff[256];
  ssize_t nread;
  nread = read(fd, buff, 256);
  if (nread == -1) {
    strerror(nread);
    exit(EXIT_FAILURE);
  }
  if (nread == 0) {
    printf("EOF occured.");
  }
  
  write(STDOUT_FILENO, buff, nread);
  close(fd);
  exit(EXIT_SUCCESS);

  return 0;
}
