#pragma once

#include <chrono>
#include <functional>
#include <iomanip>
#include <string>

class Task {
public:
  Task();
  Task(int id, std::chrono::system_clock::time_point start_time,
       std::chrono::milliseconds duration, int priority,
       std::function<void()> func);

  Task(Task &&) = default;
  Task(const Task &) = default;
  Task &operator=(Task &&) = default;
  Task &operator=(const Task &) = default;

  void operator()() const;
  bool operator<(const Task &other) const;
  int priority() const;

  int id() const;
  std::chrono::system_clock::time_point start_time() const;
  std::chrono::milliseconds duration() const;

private:
  int id_;
  std::chrono::system_clock::time_point start_time_;
  std::chrono::milliseconds duration_;
  int priority_;

  std::function<void()> func_;
};
