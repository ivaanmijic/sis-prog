#include "../include/Utils.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

[[noreturn]] void fatal(const std::string &prefix, const std::string &msg) {
  std::cerr << prefix << msg << "\n";
  std::cerr.flush();
  std::exit(EXIT_FAILURE);
}
