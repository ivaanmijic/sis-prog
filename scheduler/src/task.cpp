#include "../include/task.h"
#include <chrono>

Task::Task()
    : id_(0), startTime_(std::chrono::system_clock::now()),
      duration_(std::chrono::milliseconds{0}), priority_(0), work_([] {}) {}

Task::Task(int id, std::chrono::system_clock::time_point startTime,
           std::chrono::milliseconds duration, int priority, Work work)
    : id_(id), startTime_(startTime), duration_(duration), priority_(priority),
      work_(std::move(work)) {}

void Task::operator()() const { work_(); }

int Task::priority() const { return priority_; }

int Task::id() const { return id_; }

std::chrono::system_clock::time_point Task::startTime() const {
  return startTime_;
}

std::chrono::milliseconds Task::duration() const { return duration_; }
