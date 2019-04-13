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
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include "Component.hpp"
#include "ITimerListener.hpp"
#include "Event.hpp"

namespace icc {

class Timer : public Event<void(const TimerEvents &)> {
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
  Timer(boost::asio::io_service * service_)
      : timer_(*service_) {
  }

  Timer(std::shared_ptr<boost::asio::io_service> service_)
      : timer_(*service_) {
  }

 public:
  /**
   * Enable continuous mode
   */
  void enableContinuous() {
    counter_ = Infinite;
  }

  /**
   * Disable continuous mode
   */
  void disableContinuous() {
    if (Infinite == counter_) {
      counter_ = OneTime;
    }
  }

  /**
   * Setting number of repetitions
   * @param number Number of repetition
   */
  void setNumberOfRepetition(const int32_t &number) {
    if (number < 0) {
      counter_ = Infinite;
    } else {
      counter_ = number;
    }
  }

  /**
   * Setting interval mode for the timer
   * @param _duration Timeout duration in boost::posix_time::time_duration
   */
  void setInterval(const boost::posix_time::time_duration &_duration) {
    duration_ = std::chrono::nanoseconds(_duration.total_nanoseconds());
  }

  /**
   * Setting interval mode for the timer
   * @param _duration Timeout duration in std::chrono::nanoseconds
   */
  void setInterval(const std::chrono::nanoseconds &_duration) {
    duration_ = _duration;
  }

  /**
   * Method is used to start async waiting timer
   */
  void start() {
    timer_.expires_from_now(duration_);
    operator()(TimerEvents::STARTED);
    timer_.async_wait(std::bind(&Timer::timerExpired, this, std::placeholders::_1));
  }

  /**
   * Method is used to stop waiting timer
   */
  void stop() {
    operator()(TimerEvents::STOPPED);
    timer_.cancel();
  }

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template<typename _Listener>
  void addListener(_Listener *_listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerListener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if (_listener) {
      this->connect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template<typename _Listener>
  void addListener(std::shared_ptr<_Listener> _listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerListener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if (_listener) {
      this->connect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  template<typename _Listener>
  void removeListener(_Listener *_listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerListener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if (_listener) {
      this->disconnect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  template<typename _Listener>
  void removeListener(std::shared_ptr<_Listener> _listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerListener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if (_listener) {
      this->disconnect(&_Listener::processEvent, _listener);
    }
  }

 protected:
  /**
   * Method that handle Timer expire event
   * @param _error
   */
  virtual void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      operator()(TimerEvents::EXPIRED);
      if (counter_ > 0) {
        --counter_;
      }
      if (Infinite == counter_ || counter_ > 0) {
        start();
      }
    }
  }

 protected:
  int32_t counter_ = OneTime;
  std::chrono::nanoseconds duration_ = std::chrono::nanoseconds::zero();
  boost::asio::steady_timer timer_;
};

}

#endif //ICC_TIMER_HPP
