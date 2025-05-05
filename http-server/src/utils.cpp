#include "../include/utils.h"
#include "../include/error.h"
#include <cstdlib>
#include <iostream>
#include <string>

[[noreturn]] void fatal(const std::string &msg) {
  std::cerr << "[FATAL] " << msg << "\n";
  std::cerr.flush();
  std::exit(EXIT_FAILURE);
}

std::string validateArgs(int argc, char **argv) {
  if (argc < 2) {
    throw Error::Arg("Error: missing port argument.\n"
                     "Usage: " +
                     std::string(argv[0]) + " <port>");
  }

  if (argc > 2) {
    throw Error::Arg("Error: too many arguments\n"
                     "Usage: " +
                     std::string(argv[0]) + " <port>");
  }

  return std::string(argv[1]);
}
