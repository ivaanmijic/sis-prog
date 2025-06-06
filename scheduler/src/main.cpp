#include "../include/log.h"
#include "../include/scheduler.h"

std::mutex Log::log_mtx;

int main(int argc, char *argv[]) {

  // MARK: - Test 1
  std::cout << "    ---- TEST 1 ----   " << std::endl;
  auto now = std::chrono::system_clock::now();
  Scheduler scheduler1(2);

  Task task1(1, now + std::chrono::milliseconds(1000),
             std::chrono::milliseconds(3000), 2,
             [] { Log::print("Task 1 has done some work.\n"); });
  Task task2(2, now + std::chrono::milliseconds(1000),
             std::chrono::milliseconds(1000), 1,
             [] { Log::print("Task 2 has done some work.\n"); });
  Task task3(3, now + std::chrono::milliseconds(1500),
             std::chrono::milliseconds(2000), 3,
             [] { Log::print("Task 3 has done some work.\n"); });
  Task task4(4, now + std::chrono::milliseconds(1500),
             std::chrono::milliseconds(1000), 1,
             [] { Log::print("Task 3 has done some work.\n"); });

  scheduler1.addTask(task1);
  scheduler1.addTask(task2);
  scheduler1.addTask(task3);
  scheduler1.addTask(task4);

  std::this_thread::sleep_for(std::chrono::seconds(5));

  // MARK: - Test 2
  std::cout << "    ---- TEST 2 ----   " << std::endl;
  now = std::chrono::system_clock::now();
  Scheduler scheduler2;

  for (int i = 0; i < 10; ++i) {
    int priority = 10 - i;
    auto start = now + std::chrono::milliseconds(i % 3 * 1000);
    auto duration = std::chrono::milliseconds(500 + i * 100);

    scheduler2.addTask(Task(i, start, duration, priority, [] {}));
  }

  return 0;
}
