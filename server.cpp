#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <regex>

using namespace std;

int PORT = 3001;

static int checkStep(int res) {
  if (res == -1) {
    cout << "Error: " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  return res;
}

namespace Task {
  struct Base {
    string helloMsg = "Give me array of less than 10 integers separated by space \' \'.";
    string name = "Base";

    string call(string args) { return args; };

    string const getName() { return this->name; };

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

    Sum() { this->name = "Sum"; }
  };

  struct Mult : Base {
    string call(std::string args) { 
      parseArgs(args);

      int product = 1;
      for ( int el : this->parsedArgs ) product *= el;

      return to_string(product);
    }

    Mult() { this->name = "Mult"; }
  };

  struct Max : Base {
    string call(std::string args) { 
      parseArgs(args);

      return to_string( *max_element(this->parsedArgs.begin(), this->parsedArgs.end()) );
    }

    Max() { this->name = "Max"; }
  };
}

string const getCurrTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[8];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%T", &tstruct);
    return buf;
};

struct StateMashine {
  enum states { CHOOSE_TASK, ENTER_ARGS, CALCULATE_RES };
  states current_state;
  states getCurrentState() { return current_state; };
  void nextStep() { current_state = static_cast<states>((current_state+1) % (CALCULATE_RES + 1)); };
};

string buildResponse(StateMashine* state, string request, vector<Task::Base*> tasks) {
  string response;

  switch (state->getCurrentState()) {
    case StateMashine::CHOOSE_TASK:
      response.append("Enter number to select the task:\n");
      int idx;
      idx = 0;
      for(const Task::Base* task : tasks) 
        response.append(to_string(idx++)).append(": ").append(task->name).append("\n");
      response.append("0: to finish\n");
      break;
    case StateMashine::ENTER_ARGS:
      cout << request;
         break;
    case StateMashine::CALCULATE_RES:
         break;
  }
  
  return response;
};

int main()
{
  // init server
  int server = checkStep( socket(AF_INET, SOCK_STREAM, 0) );
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  inet_aton("127.0.0.1", &addr.sin_addr);

  // init awailable tasks
  vector<Task::Base*> availableTasks = { new Task::Sum, new Task::Mult, new Task::Max };

  int bind_res = checkStep( ::bind(server, (struct sockaddr *)&addr, sizeof(addr)) );
  int listen_res = checkStep( listen(server, 5) );
  bool is_listening = bind_res == 0 && listen_res == 0 ; 
  if ( is_listening ) std::cout << "Listening on " << PORT << " ..." << std::endl;

  // step 1: give the list of available tasks to client
  // step 2: get clients choice (index in array), response with task->helloMsg 
  // step 3: get string of args, run task.call(args), response with result

  // get requests
  socklen_t addrlen = sizeof(addr);
  int fd;
  char buff[256];
  string response, current_time;
  StateMashine state;

  while ( true ) {
    cout << "current state: " << state.getCurrentState() << endl;
    fd = checkStep( accept(server, (struct sockaddr *)&addr, &addrlen) );
    checkStep( read(fd, buff, 256) );
    // if (nread == 0) std::cout << "End of file" << std::endl;
    // send response
    response = buildResponse(&state, buff, availableTasks);
    current_time = getCurrTime();
    cout << '[' << current_time << "] client: " << buff << endl;
    checkStep( write(fd, response.c_str(), sizeof(response)) ); // number of written bytes
    current_time = getCurrTime();
    cout << '[' << current_time << "] server: " << response << endl;

    state.nextStep();
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
