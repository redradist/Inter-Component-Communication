//
// Created by redra on 08.06.19.
//

#ifndef POSIX_TIMER_HPP
#define POSIX_TIMER_HPP

#include <memory>
#include <chrono>

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
  ~Timer() = default;

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
  void setInterval(std::chrono::nanoseconds _duration);

  /**
   *
   */
  bool start();
  /**
   *
   */
  bool stop();

 private:
  friend class EventLoop;
  // NOTE(redra): Forward declaration to use pimpl
  class TimerImpl;

  explicit Timer(std::shared_ptr<TimerImpl> implPtr);

  std::shared_ptr<TimerImpl> impl_ptr_;
};

}

}

#endif //POSIX_TIMER_HPP
