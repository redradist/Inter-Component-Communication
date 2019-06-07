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

#include <icc/os/EventLoop.hpp>

#include "OSObject.hpp"
#include "TimerImpl.hpp"

namespace icc {

namespace os {

struct Timer::TimerImpl::InternalData {
};

Timer::TimerImpl::TimerImpl(const OSObject & timerObject)
  : timer_object_{timerObject} {
}

Timer::TimerImpl::~TimerImpl() {
}

/**
 * Enable continuous mode
 */
void Timer::TimerImpl::enableContinuous() {
  counter_.store(Infinite, std::memory_order_release);
}

/**
 * Disable continuous mode
 */
void Timer::TimerImpl::disableContinuous() {
  int32_t prevMode = Infinite;
  if (!counter_.compare_exchange_strong(prevMode, OneTime)) {
    // TODO(redra): Exception should be thrown
  }
}

void Timer::TimerImpl::setNumberOfRepetition(const int32_t &number) {
  if (number < 0) {
    counter_.store(Infinite, std::memory_order_release);
  } else {
    counter_.store(number, std::memory_order_release);
  }
}

void Timer::TimerImpl::start() {
  itimerspec ival;
  ival.it_value.tv_sec = duration_.count() / 1000000000LL;
  ival.it_value.tv_nsec = duration_.count() % 1000000000LL;
  ival.it_interval.tv_sec = 0;
  ival.it_interval.tv_nsec = 0;
  timerfd_settime(timer_object_.fd_, 0, &ival, nullptr);
}

void Timer::TimerImpl::stop() {

}

void Timer::TimerImpl::onTimerExpired(const OSObject & _) {
  std::cout << "Timer expired !!" << std::endl;
  uint64_t numberExpired;
  read(timer_object_.fd_, &numberExpired, sizeof(numberExpired));
  std::cout << "Timer expired: numberExpired = " << numberExpired << " !!" << std::endl;
  if (counter_.load() == Infinite) {
    itimerspec ival;
    ival.it_value.tv_sec = duration_.count() / 1000000000LL;
    ival.it_value.tv_nsec = duration_.count() % 1000000000LL;
    ival.it_interval.tv_sec = 0;
    ival.it_interval.tv_nsec = 0;
    timerfd_settime(timer_object_.fd_, 0, &ival, nullptr);
  }
}

}

}
