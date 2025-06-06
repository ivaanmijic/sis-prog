#include <chrono>
#include <iostream>
#include <thread>

void do_work() {
  std::cout << "Working in thread..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "Thread finished.\n";
}

int main() {
  std::thread t{do_work};

  std::cout << "Maint thread doing other work.\n";

  t.detach();
  std::cout << "Main thread: t has joined\n";
  return 0;
}
