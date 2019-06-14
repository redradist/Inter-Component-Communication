//
// Created by redra on 01.06.19.
//

#ifndef ICC_OS_POSIX_TIMER_HPP
#define ICC_OS_POSIX_TIMER_HPP

#include <memory>
#include <chrono>

#include <icc/ITimerListener.hpp>
#include <icc/os/EventLoop.hpp>
#include <icc/os/Timer.hpp>

#include "OSObject.hpp"

namespace icc {

namespace os {

struct OSObject;

class Timer::TimerImpl {
 public:
  ~TimerImpl() = default;

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
   * @param _duration Timeout duration in std::chrono::nanoseconds
   */
  void setInterval(std::chrono::nanoseconds _duration);

  bool start();
  bool stop();

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(std::shared_ptr<ITimerListener> _listener);

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(ITimerListener * _listener);

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(std::shared_ptr<ITimerListener> _listener);

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(ITimerListener * _listener);

 private:
  friend class EventLoop;

  explicit TimerImpl(const OSObject & timerObject);
  void onTimerExpired(const OSObject & osObject);

  std::mutex mutex_;
  OSObject timer_object_{-1};
  std::atomic_bool execute_{false};
  std::atomic_int32_t counter_{OneTime};
  std::vector<ITimerListener*> listeners_ptr_;
  std::vector<std::weak_ptr<ITimerListener>> listeners_;
  std::chrono::nanoseconds duration_ = std::chrono::nanoseconds::zero();
};

}

}

#endif //ICC_OS_POSIX_TIMER_HPP
