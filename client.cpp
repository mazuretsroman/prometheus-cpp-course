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

int checkStep(int res) {
  if (res == -1) {
    std::cout << "Error: " << std::strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }
  return res;
}

int main()
{
  // init client
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3001);
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

  // send request
  checkStep( connect(fd, (struct sockaddr *) &addr, sizeof(addr)) );
  checkStep( write(fd, "Hello", 6) );

  // receive response
  char buff[256];
  ssize_t nread;
  nread = checkStep( read(fd, buff, 256) );
  if (nread == 0)
    printf("EOF occured.");
  
  checkStep( write(STDOUT_FILENO, buff, nread) );

  // stop client
  close(fd);
  exit(EXIT_SUCCESS);

  return 0;
}
