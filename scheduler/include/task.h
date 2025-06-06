#pragma once

#include <chrono>
#include <functional>
#include <iomanip>
#include <string>

typedef std::function<void()> Work;

class Task {
public:
  Task();
  Task(int id, std::chrono::system_clock::time_point startTime,
       std::chrono::milliseconds duration, int priority, Work work);

  Task(Task &&) = default;
  Task(const Task &) = default;
  Task &operator=(Task &&) = default;
  Task &operator=(const Task &) = default;

  void operator()() const;

  int id() const;
  std::chrono::system_clock::time_point startTime() const;
  std::chrono::milliseconds duration() const;
  int priority() const;

private:
  int id_;
  std::chrono::system_clock::time_point startTime_;
  std::chrono::milliseconds duration_;
  int priority_;

  Work work_;
};
