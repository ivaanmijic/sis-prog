#include "../include/Port.h"
#include "../include/error.h"

#include <regex>
#include <string>

Port::Port(const std::string &s) : portNumber(parsePort(s)) {}

int Port::parsePort(const std::string &s) {
  static const std::regex portRx{R"(^\d{1,5}$)"};
  if (!std::regex_match(s, portRx))
    throw Error::Port("Port format error: Port must consist of 1 to 5 digits");

  long p = std::stol(s);
  if (p < 0 || p >= 65535) {
    throw Error::Port("Port range error: Port must be between 0 and 65535");
  }
  return static_cast<int>(p);
}
