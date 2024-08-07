#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/_endian.h>
#include <sys/_types/_socklen_t.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int handleStep(int res) {
  if (res == -1) {
    std::cout << "Error: " << std::strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }
  return res;
}

int main()
{
  // init server
  int server = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3001);
  inet_aton("127.0.0.1", &addr.sin_addr);
  handleStep( bind(server, (struct sockaddr *)&addr, sizeof(addr)) );
  handleStep( listen(server, 5) );
  std::cout << "Listening 3001..." << std::endl;

  // get request
  socklen_t addrlen = sizeof(addr);
  int fd = accept(server, (struct sockaddr *)&addr, &addrlen);
  ssize_t nread;
  char buff[256];
  nread = handleStep( read(fd, buff, 256) );
  if (nread == 0) std::cout << "End of file" << std::endl;

  // send response
  write(STDOUT_FILENO, buff, nread);
  write(fd, buff, nread);

  // stop server
  sleep(1);
  close(fd);
  close(server);

  return 0;
}
