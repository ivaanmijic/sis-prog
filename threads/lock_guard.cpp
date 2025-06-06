#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int counter{0};
std::mutex mtx;

void increment_counter() {
  for (auto i = 0; i < 10'000; ++i) {
    std::lock_guard<std::mutex> lck{mtx};
    counter++;
  }
}

int main() {
  std::vector<std::thread> threads;
  for (auto i = 0; i < 5; ++i) {
    threads.emplace_back(increment_counter);
  }

  for (std::thread &t : threads) {
    t.join();
  }

  std::cout << "Final counter value: " << counter << std::endl;
  return 0;
}
