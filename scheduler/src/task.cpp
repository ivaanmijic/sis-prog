#include "../include/task.h"

Task::Task(int priority, std::function<void()> func)
    : priority_(priority), func_(std::move(func)) {}

void Task::operator()() const { func_(); }

bool Task::operator<(const Task &other) const {
  return priority_ < other.priority_;
}

int Task::priority() const { return priority_; }
