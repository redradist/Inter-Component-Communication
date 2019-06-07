//
// Created by redra on 01.06.19.
//

#ifndef ICC_OS_POSIX_TIMER_HPP
#define ICC_OS_POSIX_TIMER_HPP

#include <memory>
#include <chrono>

#include <icc/ITimerListener.hpp>
#include "EventLoop.hpp"

namespace icc {

namespace os {

class Timer {
 public:
  enum : int32_t {
    /**
     * Should be used for setting continuous mode
     */
    Infinite = -1,
    /**
     * Should be used for setting one time mode
     */
    OneTime = 0,
  };

  static std::shared_ptr<Timer> createTimer();
  ~Timer();

  /**
   * Enable continuous mode
   */
  void enableContinuous();

  /**
   * Disable continuous mode
   */
  void disableContinuous();

  /**
   * Setting number of repetitions
   * @param number Number of repetition
   */
  void setNumberOfRepetition(const int32_t &number);

  /**
   * Setting interval mode for the timer
   * @param _duration Timeout duration in boost::posix_time::time_duration
   */
  template<typename _Rep, typename _Period>
  void setInterval(const std::chrono::duration<_Rep, _Period> &_duration) {
    duration_ = _duration;
  }

  void start();
  void stop();

 private:
  friend class EventLoop;
  struct InternalData;

  explicit Timer(const EventLoop::OSObject & timerObject);
  void onTimerExpired(int fd);

  EventLoop::OSObject * const timer_object_ = nullptr;
  std::atomic_int32_t counter_{OneTime};
  std::chrono::nanoseconds duration_ = std::chrono::nanoseconds::zero();
};

}

}

#endif //ICC_OS_POSIX_TIMER_HPP
