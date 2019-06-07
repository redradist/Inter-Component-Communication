#include <iostream>
#include <thread>
#include <icc/os/Timer.hpp>
#include <functional>

int main() {
  auto timer = icc::os::Timer::createTimer();

  timer->setInterval(std::chrono::seconds(10));
  timer->start();
  std::this_thread::sleep_for(std::chrono::seconds(20));
  std::cout << "Hello, World!" << std::endl;
  return 0;
}
