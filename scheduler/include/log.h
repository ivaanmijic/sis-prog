#pragma once

#include <iomanip>
#include <iostream>
#include <mutex>

class Log {
public:
  template <typename... Args> static void print(Args &&...args) {
    std::lock_guard<std::mutex> lock{log_mtx};
    (std::cout << ... << args) << std::endl;
  }

private:
  static std::mutex log_mtx;
};
