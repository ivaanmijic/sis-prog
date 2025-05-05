#pragma once

#include <stdexcept>

namespace Error {

struct Port : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct Arg : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

} // namespace Error
