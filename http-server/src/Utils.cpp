#include "../include/Utils.h"
#include "../include/Error.h"
#include <cstdlib>
#include <iostream>
#include <string>

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
