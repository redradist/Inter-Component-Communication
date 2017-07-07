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

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "IComponent.hpp"
#include "ITimerLisener.hpp"

namespace icc {

class Timer
    : protected virtual IComponent,
      public Event<void(const TimerEvents &)> {
 public:
  /**
   * Should be used for setting continuous mode
   */
  static const int32_t Infinite = -1;
  /**
   * Should be used for setting one time mode
   */
  static const int32_t OneTime = 0;
 public:
  Timer(IComponent *_parent)
      : IComponent(_parent),
        timer_(*service_) {
  }

 public:
  /**
   * Enable continuous mode
   */
  void enableContinuous() {
    push([=] {
      counter_ = Infinite;
    });
  }

  /**
   * Disable continuous mode
   */
  void disableContinuous() {
    push([=] {
      if (Infinite == counter_) {
        counter_ = OneTime;
      }
    });
  }

  /**
   * Setting number of repetitions
   * @param number Number of repetition
   */
  void setNumberOfRepetition(const int32_t &number) {
    push([=] {
      if (number < 0) {
        counter_ = Infinite;
      } else {
        counter_ = number;
      }
    });
  }

  /**
   * Setting interval mode for the timer
   * @param _duration Timeout duration
   */
  void setInterval(const boost::posix_time::time_duration &_duration) {
    push([=] {
      duration_ = _duration;
    });
  }

  /**
   * Method is used to start async waiting timer
   */
  void start() {
    push([=] {
      timer_.expires_from_now(duration_);
      operator()(TimerEvents::STARTED);
      timer_.async_wait(std::bind(&Timer::timerExpired, this, std::placeholders::_1));
    });
  }

  /**
   * Method is used to stop waiting timer
   */
  void stop() {
    push([=] {
      operator()(TimerEvents::STOPED);
      timer_.cancel();
    });
  }

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template<typename _Listener>
  void addListener(_Listener *_listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerLisener, _Listener>::value,
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
    static_assert(std::is_base_of<ITimerLisener, _Listener>::value,
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
    static_assert(std::is_base_of<ITimerLisener, _Listener>::value,
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
    static_assert(std::is_base_of<ITimerLisener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if (_listener) {
      this->disconnect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Overrided function to specify exit event
   */
  void exit() override {
    push([=] {
      IComponent::exit();
      stop();
    });
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
      if (Infinite == counter_ ||
          counter_ > 0) {
        start();
      }
    }
  }

 protected:
  int32_t counter_;
  boost::posix_time::time_duration duration_;
  boost::asio::deadline_timer timer_;
};

}

#endif //ICC_TIMER_HPP
