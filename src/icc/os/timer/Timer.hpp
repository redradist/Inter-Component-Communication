//
// Created by redra on 08.06.19.
//

#ifndef POSIX_TIMER_HPP
#define POSIX_TIMER_HPP

#include <memory>
#include <chrono>

#include <icc/ITimer.hpp>
#include <icc/os/timer/ITimerListener.hpp>
#include <icc/_private/api.hpp>

namespace icc {

namespace os {

class ITimerListener;

class ICC_PUBLIC Timer : public icc::ITimer {
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

  static std::shared_ptr<Timer> __cdecl createTimer();
  ~Timer() = default;

  /**
   * Enable continuous mode
   */
  void enableContinuous() override;

  /**
   * Disable continuous mode
   */
  void disableContinuous() override;

  /**
   * Setting number of repetitions
   * @param number Number of repetition
   */
  void setNumberOfRepetition(const int32_t &number) override;

  /**
   * Setting interval mode for the timer
   * @param _duration Timeout duration in boost::posix_time::time_duration
   */
  void setInterval(std::chrono::nanoseconds _duration) override;

  /**
   * Method is used to start async waiting timer
   */
  bool start() override;

  /**
   * Method is used to stop waiting timer
   */
  bool stop() override;

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
  class TimerImpl;

  explicit Timer(std::shared_ptr<TimerImpl> implPtr);

  std::shared_ptr<TimerImpl> impl_ptr_;
};

}

}

#endif //POSIX_TIMER_HPP
