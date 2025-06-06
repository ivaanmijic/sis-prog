#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
int data{0};
bool ready{false};

void process_data() {
  std::unique_lock<std::mutex> lck{mtx};

  data = 42;
  ready = true;
  std::cout << "Data processed. Data: " << data << std::endl;
  lck.unlock();
}

int main() {
  std::thread t{process_data};

  {
    std::unique_lock<std::mutex> main_lck(mtx, std::defer_lock);
    main_lck.lock();
    std::cout << "main thread locked.\n";
  }

  t.join();
  std::cout << "Main thread: Data is " << data << ", Ready: " << ready
            << std::endl;
  return 0;
}
