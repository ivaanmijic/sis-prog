#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
std::string data;
bool ready{false};
bool processed{false};

void workerThread() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, []() { return ready; });

  std::cout << "Worker thread is processing data.\n";
  data += " after processing";

  processed = true;
  std::cout << "Worker thread signals data processing completed.\n";

  lock.unlock();
  cv.notify_one();
}

int main() {
  std::thread worker(workerThread);

  data = "Example data";
  {
    std::lock_guard<std::mutex> lock{mtx};
    ready = true;
    std::cout << "main() signals data ready for processing\n";
  }

  {
    std::unique_lock<std::mutex> lock{mtx};
    cv.wait(lock, [] { return processed; });
    std::cout << "Back in main(), data = " << data << std::endl;
  }

  worker.join();
}
