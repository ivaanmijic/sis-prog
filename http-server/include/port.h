#pragma once

#include <string>

class Port {
public:
  explicit Port(const std::string &s);
  unsigned int get() const noexcept { return portNumber; }

private:
  unsigned int portNumber;
  static int parsePort(const std::string &s);
};
