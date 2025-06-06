#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::vector<int> brojevi;
std::mutex mtx;

void add_numbers(int start, int count) {
  for (auto i = 0; i < count; ++i) {
    // std::lock_guard<std::mutex> lock(mtx);
    brojevi.push_back(start + i);
    std::cout << "Thread: " << std::this_thread::get_id() << " added "
              << start + i << std::endl;
  }
}

int main(int argc, char *argv[]) {
  std::thread t1(add_numbers, 0, 5);
  std::thread t2(add_numbers, 100, 5);

  t1.join();
  t2.join();

  std::cout << "\nFinal contents of brojevi:\n";
  for (const auto &num : brojevi)
    std::cout << num << " ";
  std::cout << std::endl;

  return 0;
}
