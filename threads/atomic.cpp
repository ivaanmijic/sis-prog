#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

std::atomic<int> counter{0};

void increment() {
  for (int i = 0; i < 100'000; ++i) {
    counter++;
  }
}

int main(int argc, char *argv[]) {
  std::thread t1{increment};
  std::thread t2{increment};

  t1.join();
  t2.join();

  std::cout << "Final atomic counter vlau: " << counter << std::endl;
  return 0;
}
