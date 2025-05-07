#include "../include/Utils.h"
#include "../include/Error.h"
#include "../include/Logger.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

constexpr int MAXLINE = 1024;

[[noreturn]] void fatal(const std::string &prefix, const std::string &msg) {
  std::cerr << prefix << msg << "\n";
  std::cerr.flush();
  std::exit(EXIT_FAILURE);
}

std::string validateArgs(int argc, char **argv) {
  if (argc < 2) {
    throw Error::Arg("missing port argument.\n"
                     "Try: " +
                     std::string(argv[0]) + " <port>");
  }

  if (argc > 2) {
    throw Error::Arg("too many arguments\n"
                     "Try: " +
                     std::string(argv[0]) + " <port>");
  }

  return std::string(argv[1]);
}

std::string execute(const std::vector<const char *> &argv) {
  int pipefd[2];
  char buffer[MAXLINE];

  if (pipe(pipefd) < 0) {
    throw std::runtime_error("Pipe failure");
  }

  pid_t pid = fork();
  if (pid < 0) {
    close(pipefd[0]);
    close(pipefd[1]);
    throw std::runtime_error("Fork failure");
  }

  if (pid == 0) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    dup2(pipefd[1], STDERR_FILENO);
    close(pipefd[1]);

    execvp(argv[0], const_cast<char *const *>(argv.data()));
    perror("execvp");
    _exit(EXIT_FAILURE);
  }

  close(pipefd[1]);
  std::string output;
  ssize_t nBytes;
  while ((nBytes = read(pipefd[0], buffer, MAXLINE - 1)) > 0) {
    output.append(buffer, nBytes);
  }

  if (nBytes < 0) {
    close(pipefd[0]);
    throw std::runtime_error("Read error");
  }
  close(pipefd[0]);

  int status;
  waitpid(pid, &status, 0);

  return output;
}
