#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

std::mutex mtx;
std::condition_variable cv_producer;
std::condition_variable cv_consumer;
std::queue<int> buffer;
const unsigned int MAXBUFF = 10;

std::atomic<bool> producer_finished(false);
int total_items_to_produce{50};
std::atomic<int> items_produced_count{0};

void producer_task(int id) {
  int items_per_producer = total_items_to_produce / 2;
  if (id == 1)
    items_per_producer = total_items_to_produce - (total_items_to_produce / 2);

  for (int i = 0; i < items_per_producer; ++i) {
    int item_to_produce = items_produced_count.fetch_add(1);

    if (item_to_produce >= total_items_to_produce) {
      items_produced_count.fetch_sub(1);
      break;
    }

    {
      std::unique_lock<std::mutex> lock{mtx};
      cv_producer.wait(lock, [&] { return buffer.size() < MAXBUFF; });

      buffer.push(item_to_produce);
      std::cout << "Producer " << id << " produced: " << item_to_produce
                << ". Buffer size: " << buffer.size() << std::endl;
    }
    cv_consumer.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  std::cout << "PRODUCER " << id << " FINISHED ITS PART\n";
}

void consumer_task(int id) {
  while (true) {
    {
      std::unique_lock<std::mutex> lock{mtx};
      cv_consumer.wait(
          lock, [&] { return !buffer.empty() || producer_finished.load(); });

      if (buffer.empty() && producer_finished.load()) {
        std::cout
            << "Consumer " << id
            << "detected all producers finished and buffer empty. Exiting."
            << std::endl;
        break;
      }

      int data = buffer.front();
      buffer.pop();
      std::cout << "Consumer " << id << "consumed: " << data
                << ". Buffer size: " << buffer.size() << std::endl;
    }

    cv_producer.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

int main() {
  const int num_producers{2};
  const int num_consumers{3};

  std::vector<std::thread> producers;
  std::vector<std::thread> consumers;

  std::cout << "Starting " << num_producers << " producers and "
            << num_consumers << " consumers.\n";

  for (int i = 0; i < num_producers; ++i) {
    producers.emplace_back(producer_task, i + 1);
  }

  for (int i = 0; i < num_consumers; ++i) {
    producers.emplace_back(consumer_task, i + 1);
  }

  for (auto &p : producers) {
    p.join();
  }

  cv_consumer.notify_all();
  cv_producer.notify_all();

  for (auto &c : consumers) {
    c.join();
  }

  std::cout << "All threads finished. Final buffer size: " << buffer.size()
            << std::endl;
  return 0;
}
