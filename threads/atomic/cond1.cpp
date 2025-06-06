#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void print_id(int id) {
  std::unique_lock<std::mutex> lck(mtx);
  cv.wait(lck, [&]() { return ready; });
  std::cout << "Thrad " << id << " is working.\n";
}

void go() {
  std::unique_lock<std::mutex> lck{mtx};
  ready = true;
  cv.notify_all();
  std::cout << "nesto" << std::endl;
}

int main(int argc, char *argv[]) {
  std::thread threads[10];

  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(print_id, i);
  }

  std::cout << "Ready to work...\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));
  go();

  for (auto &t : threads)
    t.join();

  return 0;
}
