/**
 * @file Timer.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Thread safe version of Timer class
 * It is based on boost::asio::deadline_timer
 * @copyright MIT License. Open source:
 */

#ifndef ICC_TIMER_HPP
#define ICC_TIMER_HPP

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "IComponent.hpp"
#include "ITimerLisener.hpp"

class Timer
  : protected IComponent,
    public Event<void(const TimerEvents &)> {
 public:
  Timer(IComponent * _parent)
    : IComponent(_parent),
      timer_(*service_) {
  }

 public:
  /**
   * Setting continuos mode for the timer
   * @param is_continuos
   */
  void setContinuos(const bool & is_continuos) {
    push([=]{
      is_continuos_ = is_continuos;
    });
  }

  /**
   * Setting interval mode for the timer
   * @param _duration
   */
  void setInterval(const boost::posix_time::time_duration & _duration) {
    push([=]{
      duration_ = _duration;
    });
  }

  /**
   * Method is used to start async waiting timer
   */
  void start() {
    push([=]{
      timer_.expires_from_now(duration_);
      operator()(TimerEvents::STARTED);
      timer_.async_wait(std::bind(&Timer::timerExpired, this, std::placeholders::_1));
    });
  }

  /**
   * Method is used to stop waiting timer
   */
  void stop() {
    push([=]{
      operator()(TimerEvents::STOPED);
      timer_.cancel();
    });
  }

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template <typename _Listener>
  void addListener(_Listener * _listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerLisener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if (_listener) {
      this->connect(
          static_cast<void(IComponent::*)(const TimerEvents &)>(
              &_Listener::processEvent),
          static_cast<IComponent*>(_listener));
    }
  }

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template <typename _Listener>
  void addListener(std::shared_ptr<_Listener> _listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerLisener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if(_listener) {
      this->connect(
          static_cast<void(IComponent::*)(const TimerEvents &)>(
              &_Listener::processEvent),
          static_cast<std::shared_ptr<IComponent>>(_listener));
    }
  }

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  template <typename _Listener>
  void removeListener(_Listener * _listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerLisener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if (_listener) {
      this->disconnect(
          static_cast<void(IComponent::*)(const TimerEvents &)>(
              &_Listener::processEvent),
          static_cast<IComponent*>(_listener));
    }
  }

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  template <typename _Listener>
  void removeListener(std::shared_ptr<_Listener> _listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerLisener, _Listener>::value,
                  "_listener is not derived from ITimerLisener");
    if(_listener) {
      this->disconnect(
          static_cast<void(IComponent::*)(const TimerEvents &)>(
              &_Listener::processEvent),
          static_cast<std::shared_ptr<IComponent>>(_listener));
    }
  }

  /**
   * Overrided function to specify exit event
   */
  void exit() override {
    push([=]{
      IComponent::exit();
      stop();
    });
  }

 protected:
  /**
   * Method that handle Timer expire event
   * @param _error
   */
  virtual void timerExpired(const boost::system::error_code& _error) {
    if (!_error) {
      operator()(TimerEvents::EXPIRED);
      if (is_continuos_) {
        start();
      }
    }
  }

 protected:
  bool is_continuos_;
  boost::posix_time::time_duration duration_;
  boost::asio::deadline_timer timer_;
};

#endif //ICC_TIMER_HPP
