#include <chrono>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx_cv;
std::condition_variable cv;
std::deque<int> buffer;
const unsigned int MAXBUFF = 5;

void producer() {
  for (int i = 0; i < 10; ++i) {
    {
      std::unique_lock<std::mutex> lock{mtx_cv};
      cv.wait(lock, [] { return buffer.size() < MAXBUFF; });

      buffer.push_back(i);
      std::cout << "Producer produced: " << i
                << ". Buffer size: " << buffer.size() << std::endl;
    }
    cv.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void consumer() {
  for (int i = 0; i < 10; ++i) {
    {
      std::unique_lock<std::mutex> lock{mtx_cv};
      cv.wait(lock, [] { return !buffer.empty(); });

      int data = buffer.front();
      buffer.pop_front();
      std::cout << "Consumer consumed: " << data
                << ". Buffer size: " << buffer.size() << std::endl;
    }
    cv.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}

int main() {
  std::thread cons_thread{consumer};
  std::thread prod_thread{producer};

  prod_thread.join();
  cons_thread.join();

  std::cout << "All thread finished.\n";
  return 0;
}
