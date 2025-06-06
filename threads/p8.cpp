#include <iostream>
#include <memory>
#include <mutex>

std::mutex mtx;

void foo(std::shared_ptr<int> p, int num) {
  for (int i = 0; i < num; ++i) {
    std::lock_guard<std::mutex> lg{mtx};
    ++*p;
  }
}
