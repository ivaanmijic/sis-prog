#pragma once

#include "threadpool.h"

class Scheduler : public ThreadPool {
public:
  using ThreadPool::ThreadPool;

  void addTask(Task &&task) { enqueue(std::move(task)); }
  void addTask(const Task &task) { enqueue(task); }
};
