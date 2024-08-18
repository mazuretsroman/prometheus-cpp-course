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

struct StateMashine {
  enum states { FETCH_TASKS, SELECT_TASK, SEND_ARGS };
  states current_state;
  void reset() { current_state = FETCH_TASKS; };
  void nextStep() { current_state = static_cast<states>((current_state+1) % (SEND_ARGS  + 1)); };
  StateMashine() : current_state(FETCH_TASKS) {};
};

std::string buildRequest(StateMashine* state) {
  std::string request;

  switch ( state->current_state ) {
    case StateMashine::FETCH_TASKS:
      request = "Hello";
      break;
    case StateMashine::SELECT_TASK:
      request = "1";
      break;
    case StateMashine::SEND_ARGS:
      request = "1 2";
      break;
  }

  return request;
}

int main()
{
  // init client
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3001);
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

  StateMashine state;
  std::string request;
  int fd;
  char buff[256];
  ssize_t nread;
  bool request_sent;

  // while ( true ) {
  for (int i = 0; i < 3; ++i) {
    std::cout << "Client state start step: " << state.current_state << std::endl;

    fd = checkStep( socket(AF_INET, SOCK_STREAM, 0) );
    checkStep( connect(fd, (struct sockaddr *) &addr, sizeof(addr)) );
    request = buildRequest(&state);
    request_sent = checkStep( write(fd, &request, sizeof(request)) ) > 0;
    if ( request_sent ) {
      std::cout << "Client sent: " << request << std::endl;
    }

    // receive response
    nread = checkStep( read(fd, buff, 256) );
    // TODO: read and append buff while nread != 0
    if (nread == 0) printf("EOF occured.");
    if (nread > 0) {
      std::cout << "Client received: " << std::string(buff) << std::endl;
    }
    close(fd);
    state.nextStep();

    std::cout << "Client state start step: " << state.current_state << std::endl;
    // if ( state.current_state == StateMashine::SEND_ARGS ) exit(EXIT_SUCCESS);
  }


  // stop client
  exit(EXIT_SUCCESS);

  return 0;
}
