#include "../include/utils.h"
#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>

[[noreturn]] void fatal(const std::string &msg) {
  std::cerr << "[FATAL] " << msg << "\n";
  std::cerr.flush();
  std::exit(EXIT_FAILURE);
}

bool isValidPort(const std::string &s) {
  static const std::regex portRx{R"([0-9]{1,5})"};
  if (!std::regex_match(s, portRx))
    return false;
  int port = std::stoi(s);
  return port >= 0 && port <= 65535;
}
