#include <iostream>
#include <thread>
#include <vector>

void foo(std::vector<int> brojevi) {
  std::cout << "Thread se izvrsava." << std::endl;
  for (const auto &el : brojevi)
    std::cout << el << std::endl;
}

int main(int argc, char *argv[]) {
  std::vector<int> brojevi{1, 2, 4, 5, 10};
  std::thread t{foo, brojevi};
  t.join();
  return 0;
}
