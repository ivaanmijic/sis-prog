#include "../include/threadpool.h"
#include "../include/log.h"
#include "../include/utils.h"
#include <mutex>

ThreadPool::ThreadPool(size_t num_threads) : stop_(false) {
  for (size_t i = 0; i < num_threads; ++i) {
    threads_.emplace_back([this] {
      while (true) {
        Task newTask;
        {
          std::unique_lock<std::mutex> lock{mutex_};
          cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

          if (stop_ && tasks_.empty())
            return;

          newTask = std::move(tasks_.top());
          tasks_.pop();
        }

        auto now = std::chrono::system_clock::now();
        if (now < newTask.startTime()) {
          std::this_thread::sleep_until(newTask.startTime());
        }

        Log::print("Task ", newTask.id(),
                   ", start time: ", timeToString(newTask.startTime()),
                   ", duration: ", newTask.duration().count(),
                   ", priority: ", newTask.priority());
        newTask();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  stop_ = true;
  cv_.notify_all();
  for (std::thread &thread : threads_) {
    if (thread.joinable())
      thread.join();
  }
}

void ThreadPool::enqueue(Task &&task) {
  {
    std::unique_lock<std::mutex> lock{mutex_};
    tasks_.push(std::move(task));
  }
  cv_.notify_one();
}

void ThreadPool::enqueue(const Task &task) {
  {
    std::unique_lock<std::mutex> lock{mutex_};
    tasks_.push(task);
  }
  cv_.notify_one();
}

// void ThreadPool::enqueue(int priority, std::function<void()> func) {
//   enqueue(Task(priority, std::move(func)));
// }
