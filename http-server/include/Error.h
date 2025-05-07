#pragma once

#include <stdexcept>
#include <string>

namespace Error {

struct Port : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct Arg : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct HTTP : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct HTTPWithStatus : public std::runtime_error {
  int status;

  HTTPWithStatus(int code, const std::string &msg)
      : std::runtime_error(msg), status(code) {}
};

} // namespace Error
