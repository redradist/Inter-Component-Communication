//
// Created by redra on 01.06.19.
//

#include <iostream>
#include <inttypes.h>
#include <vector>
#include <icc/os/EventLoop.hpp>

#include "Common.hpp"
#include "TimerImpl.hpp"

namespace icc {

namespace os {

static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);

Timer::TimerImpl::TimerImpl(const Handle & timerObject)
  : timer_queue_handle_{timerObject} {
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
    // Set a timer to call the timer routine in 10 seconds.
    auto dueTime = duration_.count() / 1000000LL;
    auto period = counter_.load(std::memory_order_acquire) > 0 ? dueTime : 0;
    return ::CreateTimerQueueTimer(
        &timer_handle_.handle_, timer_queue_handle_.handle_,
        reinterpret_cast<WAITORTIMERCALLBACK>(timerRoutine),
        this, dueTime, period, 0);
  }
  return false;
}

bool Timer::TimerImpl::stop() {
  bool timerEnabled = true;
  if (execute_.compare_exchange_strong(timerEnabled, false)) {
    return ::DeleteTimerQueueTimer(
        timer_queue_handle_.handle_,
        timer_handle_.handle_,
        nullptr
    );
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

void Timer::TimerImpl::onTimerExpired() {
  std::cout << "onTimerExpired" << std::endl;
  if (execute_.load(std::memory_order_acquire)) {
    current_counter_.fetch_add(1, std::memory_order_acq_rel);
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
    const auto currentCounter = current_counter_.load(std::memory_order_acquire);
    const auto maxCounter = counter_.load(std::memory_order_acquire);
    if (maxCounter > OneTime && currentCounter >= maxCounter) {
      stop();
    }
  }
}

void CALLBACK Timer::TimerImpl::timerRoutine(
    PVOID lpParam, BOOLEAN TimerOrWaitFired) {
  std::cout << "timerRoutine" << std::endl;
  if (lpParam == nullptr) {
    printf("TimerRoutine lpParam is nullptr\n");
  } else {
    auto timerImpl = reinterpret_cast<Timer::TimerImpl*>(lpParam);
    timerImpl->onTimerExpired();
  }
}

}

}
