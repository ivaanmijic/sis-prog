#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

struct Element {};
std::atomic<int> num_consumed(0), num_produced(0);

Element produce_element() {
  ++num_produced;
  return Element{};
}

void consume_element(Element &&el) { ++num_consumed; }

std::atomic<bool> done{false};
const int MAX = 10;
std::queue<Element> buffer;
std::mutex mtx;
std::condition_variable produce_cv;
std::condition_variable consume_cv;

void producer() {
  while (!done) {
    Element el = produce_element();
    {
      std::unique_lock<std::mutex> lck{mtx};
      while (buffer.size() == MAX) {
        if (done)
          return;
        produce_cv.wait(lck);
      }
      buffer.emplace(el);
    }
    consume_cv.notify_one();
  }
}

void consumer() {
  Element el;
  while (!done) {
    {
      std::unique_lock<std::mutex> lck{mtx};
      while (buffer.empty()) {
        if (done)
          return;
        consume_cv.wait(lck);
      }
      el = std::move(buffer.back());
      buffer.pop();
    }
    produce_cv.notify_one();
    consume_element(std::move(el));
  }
}

int main(int argc, char *argv[]) {
  std::thread t1{producer};
  std::thread t2{producer};
  std::thread t3{consumer};
  std::thread t4{consumer};

  std::this_thread::sleep_for(std::chrono::seconds{5});
  done = true;

  consume_cv.notify_all();
  produce_cv.notify_all();

  t1.join();
  t2.join();
  t3.join();
  t4.join();

  std::cout << buffer.size() << " " << num_produced << " " << num_consumed
            << std::endl;

  return 0;
}
