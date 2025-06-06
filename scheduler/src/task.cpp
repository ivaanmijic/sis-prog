#include "../include/task.h"
#include <chrono>

Task::Task()
    : id_(0), start_time_(std::chrono::system_clock::now()),
      duration_(std::chrono::milliseconds{0}), priority_(0), func_([] {}) {}

Task::Task(int id, std::chrono::system_clock::time_point start_time,
           std::chrono::milliseconds duration, int priority,
           std::function<void()> func)
    : id_(id), start_time_(start_time), duration_(duration),
      priority_(priority), func_(std::move(func)) {}

void Task::operator()() const { func_(); }

bool Task::operator<(const Task &other) const {
  if (start_time_ == other.start_time_) {
    return priority_ < other.priority_;
  }
  return start_time_ < other.start_time_;
}

int Task::priority() const { return priority_; }

int Task::id() const { return id_; }

std::chrono::system_clock::time_point Task::start_time() const {
  return start_time_;
}

std::chrono::milliseconds Task::duration() const { return duration_; }
