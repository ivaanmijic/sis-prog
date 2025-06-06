#pragma once

#include <functional>

class Task {
public:
  Task(int priority, std::function<void()> func);

  void operator()() const;
  bool operator<(const Task &other) const;
  int priority() const;

private:
  int priority_;
  std::function<void()> func_;
};
