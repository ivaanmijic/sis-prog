#include <chrono>
#include <iostream>
#include <thread>

void background_task() {
  std::cout << "Background task started.\n";
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "Background task finished.\n";
}

int main() {
  std::thread bg_thread{background_task};
  bg_thread.detach();

  std::cout << "main thread continuing without"
               "waiting for background task.\n";

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "Main thread exiting.\n";
  return 0;
}
