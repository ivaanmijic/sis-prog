#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

std::vector<int> data;
std::atomic<bool> data_ready(false);

void reader_thread() {
  while (!data_ready.load()) {
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
  }
  std::cout << "The answer=" << data[0] << std::endl;
}

void writer_thread() {
  data.push_back(42);
  data_ready = true;
}

int main(int argc, char *argv[]) {
  std::cout << std::endl;

  std::thread t1(reader_thread);
  std::thread t2(writer_thread);

  t1.join();
  t2.join();

  return 0;
}
