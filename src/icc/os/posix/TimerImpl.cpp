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

Timer::TimerImpl::TimerImpl(const OSObject & timerObject)
  : timer_object_{timerObject} {
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

void Timer::TimerImpl::setInterval(const std::chrono::nanoseconds _duration) {
  duration_ = _duration;
}

bool Timer::TimerImpl::start() {
  bool timerDisabled = false;
  if (execute_.compare_exchange_strong(timerDisabled, true)) {
    itimerspec ival;
    ival.it_value.tv_sec = duration_.count() / 1000000000LL;
    ival.it_value.tv_nsec = duration_.count() % 1000000000LL;
    ival.it_interval.tv_sec = 0;
    ival.it_interval.tv_nsec = 0;
    timerfd_settime(timer_object_.fd_, 0, &ival, nullptr);
    return true;
  }
  return false;
}

bool Timer::TimerImpl::stop() {
  bool timerEnabled = true;
  if (execute_.compare_exchange_strong(timerEnabled, false)) {
    itimerspec ival;
    ival.it_value.tv_sec = 0;
    ival.it_value.tv_nsec = 0;
    ival.it_interval.tv_sec = 0;
    ival.it_interval.tv_nsec = 0;
    timerfd_settime(timer_object_.fd_, 0, &ival, nullptr);
    return true;
  }
  return false;
}

void Timer::TimerImpl::addListener(std::shared_ptr<ITimerListener> _listener) {
  if (_listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    listeners_.push_back(_listener);
  }
}

void Timer::TimerImpl::addListener(ITimerListener * _listener) {
  if (_listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    listeners_ptr_.push_back(_listener);
  }
}

void Timer::TimerImpl::removeListener(std::shared_ptr<ITimerListener> _listener) {
  if (_listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto erased = std::remove_if(listeners_.begin(), listeners_.end(),
    [_listener](const std::weak_ptr<ITimerListener> & weakListener) {
      bool result = false;
      if (auto _observer = weakListener.lock()) {
        result = _observer == _listener;
      }
      return result;
    });
    listeners_.erase(erased);
  }
}

void Timer::TimerImpl::removeListener(ITimerListener * _listener) {
  if (_listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto erased = std::remove(listeners_ptr_.begin(), listeners_ptr_.end(), _listener);
    listeners_ptr_.erase(erased);
  }
}

void Timer::TimerImpl::onTimerExpired(const OSObject & _) {
  uint64_t numberExpired;
  read(timer_object_.fd_, &numberExpired, sizeof(numberExpired));
  if (execute_.load(std::memory_order_acquire)) {
    if (counter_.load() == Infinite) {
      itimerspec ival;
      ival.it_value.tv_sec = duration_.count() / 1000000000LL;
      ival.it_value.tv_nsec = duration_.count() % 1000000000LL;
      ival.it_interval.tv_sec = 0;
      ival.it_interval.tv_nsec = 0;
      timerfd_settime(timer_object_.fd_, 0, &ival, nullptr);
    }
    {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto listener : listeners_ptr_) {
        listener->onTimerExpired();
      }
      for (auto weakListener : listeners_) {
        if (auto listener = weakListener.lock()) {
          listener->onTimerExpired();
        }
      }
      for (auto weakListenerIter = listeners_.begin(); weakListenerIter != listeners_.end();) {
        if (weakListenerIter->expired()) {
          weakListenerIter = listeners_.erase(weakListenerIter);
        } else {
          ++weakListenerIter;
        }
      }
    }
  }
}

}

}
