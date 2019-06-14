//
// Created by redra on 08.06.19.
//

#include <iostream>
#include <utility>
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
#include <icc/os/Timer.hpp>

#include "OSObject.hpp"
#include "TimerImpl.hpp"

namespace icc {

namespace os {

std::shared_ptr<Timer> Timer::createTimer() {
  return EventLoop::getDefaultInstance().createTimer();
}

Timer::Timer(std::shared_ptr<TimerImpl> implPtr)
    : impl_ptr_{std::move(implPtr)} {
}

void Timer::enableContinuous() {
  impl_ptr_->enableContinuous();
}

void Timer::disableContinuous() {
  impl_ptr_->disableContinuous();
}

void Timer::setNumberOfRepetition(const int32_t &number) {
  impl_ptr_->setNumberOfRepetition(number);
}

void Timer::setInterval(std::chrono::nanoseconds _duration) {
  impl_ptr_->setInterval(_duration);
}

bool Timer::start() {
  return impl_ptr_->start();
}

bool Timer::stop() {
  return impl_ptr_->stop();
}

void Timer::addListener(std::shared_ptr<ITimerListener> _listener) {
  if (_listener) {
    impl_ptr_->addListener(_listener);
  }
}

void Timer::addListener(ITimerListener * _listener) {
  if (_listener) {
    impl_ptr_->addListener(_listener);
  }
}

void Timer::removeListener(std::shared_ptr<ITimerListener> _listener) {
  if (_listener) {
    impl_ptr_->removeListener(_listener);
  }
}

void Timer::removeListener(ITimerListener * _listener) {
  if (_listener) {
    impl_ptr_->removeListener(_listener);
  }
}

}

}
