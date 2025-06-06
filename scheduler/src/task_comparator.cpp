#include "../include/task_comparator.h"

bool TaskComparator::operator()(const Task &a, const Task &b) const {
  return a.startTime() != b.startTime() ? a.startTime() > b.startTime()
                                        : a.priority() > b.priority();
}
