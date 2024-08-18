#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
  int server = socket(AF_INET, SOCK_STREAM, 0);
  std::cout << "Server: " << server << std::endl;

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3001);
  inet_aton("127.0.0.1", &addr.sin_addr);


  int bind_res = bind(server, (struct sockaddr *)&addr, sizeof(addr));
  if ( bind_res == 0 ) {
    std::cout << "Bind success" << std::endl;
  } else if (bind_res == -1) {
    std::cout << "Bind failure" << std::endl;
    std::cout << "Error: " << strerror(errno) << std::endl;
  }

  int listen_res = listen(server, 1);
  if ( listen_res  == 0 ) {
    std::cout << "Listen success" << std::endl;
  } else if (listen_res == -1) {
    std::cout << "Listen failure" << std::endl;
    std::cout << "Error: " << strerror(errno) << std::endl;
  }

  socklen_t addrlen = sizeof(addr);
  int fd = accept(server, (struct sockaddr *)&addr, &addrlen);
  if ( fd == -1 ) {
    std::cout << "File descriptor failure" << std::endl;
    std::cout << "Error: " << strerror(errno) << std::endl;
  }
  ssize_t nread;
  char buff[256];
  nread = read(fd, buff, nread);
  write(STDOUT_FILENO, buff, (int)nread);
  write(fd, buff, (int)nread);

  close(fd);
  close(server);

  return 0;
}

