//
// Created by redra on 01.06.19.
//

#include <iostream>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <vector>
#include <functional>
#include "Timer.hpp"
#include "EventLoop.hpp"

namespace icc {

namespace os {

namespace posix {

Timer Timer::createTimer() {
  const int timerFd = timerfd_create(CLOCK_MONOTONIC, 0);
  return Timer{timerFd};
}

void Timer::start(const unsigned microSec) {
  itimerspec ival;
  ival.it_value.tv_sec = microSec / 1000000;
  ival.it_value.tv_nsec = 0;
  ival.it_interval.tv_sec = microSec / 1000000;
  ival.it_interval.tv_nsec = 0;
  timerfd_settime(timer_fd_, 0, &ival, nullptr);
  std::function<void(int)> callback = [this](int fd) {
    onTimerExpired(fd);
  };
  EventLoop::getDefaultInstance().registerFdEvents(timer_fd_,
                                                   EventLoop::FdEventType::READ,
                                                   *callback.target<void(*)(int)>());
}

void Timer::onTimerExpired(const int fd) {
  std::cout << "Timer expired !!" << std::endl;
  uint64_t repeated = 0;
  uint64_t numberExpired;
  read(fd, &numberExpired, sizeof(numberExpired));
  std::cout << "Timer expired: numberExpired = " << numberExpired << " !!" << std::endl;
}

Timer::Timer(const int timerFd)
    : timer_fd_{timerFd} {
}

}

}

}
