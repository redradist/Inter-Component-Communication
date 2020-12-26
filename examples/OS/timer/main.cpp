#include <iostream>
#include <thread>
#include <icc/os/timer/Timer.hpp>
#include <functional>

int main() {
  std::cout << "Hello, Windows Timer!" << std::endl;

  auto timer = icc::os::Timer::createTimer();
  timer->setInterval(std::chrono::seconds(10));
  timer->enableContinuous();
  timer->start();
  std::this_thread::sleep_for(std::chrono::seconds(25));

  std::cout << "You works, nice!" << std::endl;
  return 0;
}
