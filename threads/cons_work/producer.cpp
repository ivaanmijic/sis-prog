#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
using namespace std;

struct Element {};
atomic<int> numConsumed(0), numProduced(0);

Element produceElement() {
  ++numProduced;
  return Element{};
}

void consumeElement(Element &&el) { ++numConsumed; }

atomic<bool> done{false};
const int MAX = 10;
queue<Element> buffer;
mutex queueMutex;
condition_variable produceCV;
condition_variable consumeCV;

void producer() {
  while (!done) {
    Element el = produceElement();
    {
      unique_lock<mutex> lock{queueMutex};
      while (buffer.size() == MAX) {
        if (done)
          return;
        produceCV.wait(lock);
      }
      buffer.emplace(move(el));
    }
    consumeCV.notify_one();
  }
}

void consumer() {
  Element el;
  while (!done) {
    unique_lock<mutex> lock(queueMutex);
    while (buffer.empty()) {
      if (done)
        return;
      consumeCV.wait(lock);
    }
    el = move(buffer.back());
    buffer.pop();
  }
  produceCV.notify_one();
  consumeElement(move(el));
}

int main(int argc, char *argv[]) {
  thread t1{producer};
  thread t2{consumer};
  thread t3{producer};
  thread t4{consumer};

  this_thread::sleep_for(chrono::seconds{5});
  done = true;

  produceCV.notify_all();
  consumeCV.notify_all();

  t1.join();
  t2.join();
  t3.join();
  t4.join();

  cout << buffer.size() << " " << numProduced << " " << numConsumed << endl;

  return 0;
}
