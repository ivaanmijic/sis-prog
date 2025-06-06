#include "../include/threadpool.h"
#include "../include/log.h"
#include "../include/utils.h"

ThreadPool::ThreadPool(size_t num_threads) : stop_(false) {
  for (size_t i = 0; i < num_threads; ++i) {
    threads_.emplace_back([this] {
      while (true) {
        Task task;
        {
          std::unique_lock<std::mutex> lock{queue_mutex_};
          cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

          if (stop_ && tasks_.empty())
            return;

          task = std::move(tasks_.top());
          tasks_.pop();
        }

        auto now = std::chrono::system_clock::now();
        if (now < task.start_time()) {
          std::this_thread::sleep_until(task.start_time());
        }

        Log::print("Task ", task.id(),
                   ", start_time: ", timeToString(task.start_time()),
                   ", duration: ", task.duration().count(),
                   ", priority: ", task.priority());
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  stop_ = true;
  cv_.notify_all();
  for (std::thread &thread : threads_) {
    thread.join();
  }
}

void ThreadPool::enqueue(Task &&task) {
  {
    std::unique_lock<std::mutex> lock{queue_mutex_};
    tasks_.emplace(std::move(task));
  }
  cv_.notify_one();
}

// void ThreadPool::enqueue(int priority, std::function<void()> func) {
//   enqueue(Task(priority, std::move(func)));
// }
