#include "../include/log.h"
#include "../include/threadpool.h"

std::mutex Log::log_mtx;

int main(int argc, char *argv[]) {
  ThreadPool threadPool(10);

  for (int i = 0; i < 11; ++i) {
    auto now = std::chrono::system_clock::now();
    threadPool.enqueue(Task(i, now + std::chrono::seconds(i),
                            std::chrono::milliseconds(i * 500), i, [i] {
                              std::this_thread::sleep_for(
                                  std::chrono::milliseconds(2'000));
                              Log::print("Task ", i, " finished.");
                            }));
  }

  return 0;
}
