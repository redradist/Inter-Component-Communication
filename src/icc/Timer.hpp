/**
 * @file Timer.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Thread safe version of Timer class
 * It is based on boost::asio::deadline_timer
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_TIMER_HPP
#define ICC_TIMER_HPP

#include <chrono>
#include <icc/Component.hpp>
#include <icc/Event.hpp>
#include <icc/os/EventLoop.hpp>
#include <icc/os/timer/Timer.hpp>
#include <icc/os/timer/ITimerListener.hpp>

#include "ITimerListener.hpp"

namespace icc {

class Timer
    : public icc::ITimer
    , public icc::os::ITimerListener {
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

 public:
  Timer()
      : timer_ptr_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
    timer_ptr_->addListener(this);
  }

  Timer(icc::os::EventLoop * eventLoop)
      : timer_ptr_(eventLoop->createTimer()) {
    timer_ptr_->addListener(this);
  }

  Timer(std::shared_ptr<icc::os::EventLoop> eventLoop)
      : timer_ptr_(eventLoop->createTimer()) {
    timer_ptr_->addListener(this);
  }

  ~Timer() {
    timer_ptr_->removeListener(this);
    timer_ptr_->stop();
  }

 public:
  Event<void(const TimerEvents &)> timer_event_;

  /**
   * Enable continuous mode
   */
  void enableContinuous() override {
    timer_ptr_->enableContinuous();
  }

  /**
   * Disable continuous mode
   */
  void disableContinuous() override {
    timer_ptr_->disableContinuous();
  }

  /**
   * Setting number of repetitions
   * @param number Number of repetition
   */
  void setNumberOfRepetition(const int32_t &number) override {
    timer_ptr_->setNumberOfRepetition(number);
  }

  /**
   * Setting interval mode for the timer
   * @param _duration Timeout duration in std::chrono::nanoseconds
   */
  void setInterval(const std::chrono::nanoseconds _duration) override {
    timer_ptr_->setInterval(_duration);
  }

  /**
   * Method is used to start async waiting timer
   */
  bool start() override {
    const bool kResult = timer_ptr_->start();
    timer_event_(TimerEvents::STARTED);
    return kResult;
  }

  /**
   * Method is used to stop waiting timer
   */
  bool stop() override {
    const bool kResult = timer_ptr_->stop();
    timer_event_(TimerEvents::STOPPED);
    return kResult;
  }

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template<typename _Listener>
  void addListener(_Listener *_listener) {
    static_assert(std::is_base_of<Component, _Listener>::value,
                  "_listener is not derived from Component");
    static_assert(std::is_base_of<icc::ITimerListener, _Listener>::value,
                  "_listener is not derived from icc::ITimerLisener");
    if (_listener) {
      timer_event_.connect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template<typename _Listener>
  void addListener(std::shared_ptr<_Listener> _listener) {
    static_assert(std::is_base_of<Component, _Listener>::value,
                  "_listener is not derived from Component");
    static_assert(std::is_base_of<icc::ITimerListener, _Listener>::value,
                  "_listener is not derived from icc::ITimerLisener");
    if (_listener) {
      timer_event_.connect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  template<typename _Listener>
  void removeListener(_Listener *_listener) {
    static_assert(std::is_base_of<Component, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<icc::ITimerListener, _Listener>::value,
                  "_listener is not derived from icc::ITimerLisener");
    if (_listener) {
      timer_event_.disconnect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  template<typename _Listener>
  void removeListener(std::shared_ptr<_Listener> _listener) {
    static_assert(std::is_base_of<Component, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<icc::ITimerListener, _Listener>::value,
                  "_listener is not derived from icc::ITimerLisener");
    if (_listener) {
      timer_event_.disconnect(&_Listener::processEvent, _listener);
    }
  }

 protected:
  /**
   * Method that handle Timer expire event
   * @param _error
   */
  void onTimerExpired() override {
    timer_event_(TimerEvents::EXPIRED);
  }

 protected:
  std::shared_ptr<icc::os::Timer> timer_ptr_;
};

}

#endif //ICC_TIMER_HPP
