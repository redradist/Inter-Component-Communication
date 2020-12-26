//
// Created by redra on 01.06.19.
//

#ifndef ICC_OS_POSIX_TIMER_HPP
#define ICC_OS_POSIX_TIMER_HPP

#include <memory>
#include <chrono>

#include <icc/ITimerListener.hpp>
#include <icc/os/EventLoop.hpp>
#include <icc/os/timer/Timer.hpp>


#include "Common.hpp"

namespace icc {

namespace os {

struct Handle;

class Timer::TimerImpl {
 public:
  ~TimerImpl();

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

  explicit TimerImpl(const Handle & timerObject);
  void onTimerExpired();
  static void CALLBACK timerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);

  std::mutex mutex_;
  Handle timer_handle_{kInvalidHandle};
  Handle timer_queue_handle_{kInvalidHandle};
  std::atomic_bool execute_{false};
  std::atomic_int32_t counter_{OneTime};
  std::atomic_int32_t current_counter_{0};
  std::vector<ITimerListener*> listeners_ptr_;
  std::vector<std::weak_ptr<ITimerListener>> listeners_;
  std::chrono::nanoseconds duration_ = std::chrono::nanoseconds::zero();
};

}

}

#endif //ICC_OS_POSIX_TIMER_HPP
