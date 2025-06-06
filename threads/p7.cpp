#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

std::mutex mtx;

void foo(std::shared_ptr<int> p, int num) {
  for (int i = 0; i < num; ++i) {
    std::lock_guard<std::mutex> lock(mtx);
    ++*p;
  }
}

int main(int argc, char *argv[]) {
  auto ptr = std::make_shared<int>(0);
  std::thread t1{foo, ptr, 50'000};
  std::thread t2{foo, ptr, 10'000};

  std::cout << "ref. count: " << ptr.use_count() << std::endl;
  t1.join();
  t2.join();
  std::cout << "val: " << *ptr << " count: " << ptr.use_count() << std::endl;

  return 0;
}
