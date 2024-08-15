#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <regex>

using namespace std;

static int checkStep(int res) {
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

string const getCurrTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%T", &tstruct);
    return buf;
};

int main()
{
  // init server
  int server = checkStep( socket(AF_INET, SOCK_STREAM, 0) );
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3001);
  inet_aton("127.0.0.1", &addr.sin_addr);

  // init awailable tasks
  vector<Task::Base> availableTasks = { Task::Sum(), Task::Mult(), Task::Max() };

  int bind_res = checkStep( ::bind(server, (struct sockaddr *)&addr, sizeof(addr)) );
  int listen_res = checkStep( listen(server, 5) );
  bool is_listening = bind_res == 0 && listen_res == 0 ; 
  if ( is_listening ) std::cout << "Listening 3001..." << std::endl;

  // get request
  socklen_t addrlen = sizeof(addr);
  int fd;
  char buff[256];
  string response, current_time;
  while ( true ) {
    fd = checkStep( accept(server, (struct sockaddr *)&addr, &addrlen) );
    checkStep( read(fd, buff, 256) );
    // if (nread == 0) std::cout << "End of file" << std::endl;
    // send response
    response = buff;
    response.append(" from server.");
    current_time = getCurrTime();
    cout << '[' << current_time << "] client: " << buff << endl;
    checkStep( write(fd, response.c_str(), sizeof(response)) ); // number of written bytes
    current_time = getCurrTime();
    cout << '[' << current_time << "] server: " << response << endl;

    close(fd);
  }

  // if ( response.compare("0") == 0 ) {
  //   // stop server
  //   sleep(1);
  //   close(fd);
  //   close(server);
  // } 

  return 0;
}
