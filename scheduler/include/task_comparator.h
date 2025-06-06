#pragma once

#include "task.h"

struct TaskComparator {
  bool operator()(const Task &a, const Task &b) const;
};
