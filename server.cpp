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
#include <vector>
#include <regex>

using namespace std;

int checkStep(int res) {
  if (res == -1) {
    cout << "Error: " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  return res;
}

namespace Task {
  struct Base {
    string helloMsg = "Sum: Give me array of less than 10 integers separated by space \' \'.";

    string call(string args) { return args; };

    protected:
      vector<int> parsedArgs;

      void parseArgs(string args) {
	      regex regexp("[0-9]+");
	      auto words_begin = sregex_iterator(args.begin(), args.end(), regexp);
	      auto words_end = sregex_iterator();
	      for (sregex_iterator i = words_begin; i != words_end; ++i)
        {
          smatch match = *i;
          parsedArgs.push_back(stoi( match.str() ));
        }
      };
  };

  struct Sum : Base {
    string call(std::string args) { 
      parseArgs(args);

      int sum = 0;
      for ( int el : this->parsedArgs ) sum += el;

      return to_string(sum);
    }
  };

  struct Mult : Base {
    string call(std::string args) { 
      parseArgs(args);

      int product = 1;
      for ( int el : this->parsedArgs ) product *= el;

      return to_string(product);
    }
  };

  struct Max : Base {
    string call(std::string args) { 
      parseArgs(args);

      return to_string( *max_element(this->parsedArgs.begin(), this->parsedArgs.end()) );
    }
  };
}

int main()
{
  // init server
  int server = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3001);
  inet_aton("127.0.0.1", &addr.sin_addr);

  // init awailable tasks
  vector<Task::Base> availableTasks = { Task::Sum(), Task::Mult(), Task::Max() };

  bind(server, (struct sockaddr *)&addr, sizeof(addr));
  checkStep( listen(server, 5) );
  std::cout << "Listening 3001..." << std::endl;

  // get request
  socklen_t addrlen = sizeof(addr);
  int fd = accept(server, (struct sockaddr *)&addr, &addrlen);
  ssize_t nread;
  char buff[256];
  nread = checkStep( read(fd, buff, 256) );
  if (nread == 0) std::cout << "End of file" << std::endl;

  // send response
  std::string response = buff;
  response.append(" from server.");
  checkStep( write(STDOUT_FILENO, buff, nread) );
  checkStep( write(fd, response.c_str(), sizeof(response)) );

  // stop server
  sleep(1);
  close(fd);
  close(server);

  return 0;
}
