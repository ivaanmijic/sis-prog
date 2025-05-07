#pragma once

#include <stdexcept>
#include <string>

struct RelayError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
