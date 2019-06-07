#include <utility>

//
// Created by redra on 08.06.19.
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

Timer::~Timer() {
}

/**
 * Enable continuous mode
 */
void Timer::enableContinuous() {
  impl_ptr_->enableContinuous();
}

/**
 * Disable continuous mode
 */
void Timer::disableContinuous() {
  impl_ptr_->disableContinuous();
}

void Timer::setNumberOfRepetition(const int32_t &number) {
  impl_ptr_->setNumberOfRepetition(number);
}

void Timer::setInterval(std::chrono::nanoseconds _duration) {
  impl_ptr_->setInterval(_duration);
}

void Timer::start() {
  impl_ptr_->start();
}

void Timer::stop() {
  impl_ptr_->stop();
}

}

}
