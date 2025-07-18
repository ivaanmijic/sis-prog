#pragma once

#include "task.h"
#include "task_comparator.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
  ~ThreadPool();

  void enqueue(Task &&task);
  void enqueue(const Task &task);
  // void enqueue(int priority, std::function<void()> func);

private:
  std::vector<std::thread> threads_;
  std::priority_queue<Task, std::vector<Task>, TaskComparator> tasks_;

  std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic<bool> stop_;
};
