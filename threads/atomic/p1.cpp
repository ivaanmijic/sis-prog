#include <atomic>
#include <iostream>
#include <thread>

std::atomic<int> counter(0);

void decrement() {
  for (int i = 1000; i > 000; --i) {
    counter--;
  }
  std::cout << "counter value " << counter << std::endl;
}

int main(int argc, char *argv[]) {
  std::thread t1{decrement};
  std::thread t2{decrement};

  t1.join();
  t2.join();

  std::cout << "final counter value: " << counter << std::endl;

  return 0;
}
