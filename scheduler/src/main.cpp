#include "../include/threadpool.h"
#include <chrono>
#include <iostream>
#include <mutex>

std::mutex cout_mtx;

int main(int argc, char *argv[]) {
  ThreadPool threadPool(10);

  for (int i = 0; i < 11; ++i) {
    threadPool.enqueue(Task(i, [i] {
      std::lock_guard<std::mutex> lock{cout_mtx};
      std::cout << "Task " << i << " is running on thread "
                << std::this_thread::get_id() << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }));
  }

  return 0;
}
