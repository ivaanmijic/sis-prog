#include "../include/threadpool.h"

ThreadPool::ThreadPool(size_t num_threads) {
  for (size_t i = 0; i < num_threads; ++i) {
    threads_.emplace_back([this] {
      while (true) {
        Task task{0, [] {}};

        {
          std::unique_lock<std::mutex> lock(queue_mutex_);
          cv_.wait(lock, [this] { return !tasks_.empty() || stop; });

          if (stop && tasks_.empty())
            return;

          task = std::move(tasks_.top());
          tasks_.pop();
        }
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  stop = true;
  cv_.notify_all();
  for (std::thread &thread : threads_) {
    thread.join();
  }
}

void ThreadPool::enqueue(Task task) {
  {
    std::unique_lock<std::mutex> lock{queue_mutex_};
    tasks_.emplace(std::move(task));
  }
  cv_.notify_one();
}

void ThreadPool::enqueue(int priority, std::function<void()> func) {
  enqueue(Task(priority, std::move(func)));
}
