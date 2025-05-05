#pragma once

#include <string>

class Port {
public:
  explicit Port(const std::string &s);
  explicit Port(const Port &) = default;
  explicit Port(Port &&) = default;

  Port &operator=(Port &&) noexcept = default;
  Port &operator=(const Port &) noexcept = default;

  unsigned int get() const noexcept { return portNumber; }

private:
  unsigned int portNumber;
  static int parsePort(const std::string &s);
};
