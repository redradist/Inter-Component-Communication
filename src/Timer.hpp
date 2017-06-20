/**
 * @file Timer.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Timer class
 * @copyright MIT License. Open source:
 */

#ifndef ICC_TIMER_HPP
#define ICC_TIMER_HPP

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "IComponent.hpp"
#include "ITimerLisener.hpp"

class Timer
  : protected IComponent {
 public:
  using tTimerCallback = std::function<void(const TimerEvents &)>;
  using tListCallbacks = std::vector<tTimerCallback>;
  using tObjectAndCallbacks = std::pair<std::weak_ptr<IComponent>, tTimerCallback>;
  using tCheckedListCallbacks = std::vector<tObjectAndCallbacks>;
 public:
  Timer(IComponent * _parent)
    : IComponent(_parent),
      timer_(*service_),
      unchecked_listeners_(std::make_shared<tListCallbacks>()),
      checked_listeners_(std::make_shared<tCheckedListCallbacks>()) {
  }

 public:
  /**
   *
   * @param is_continuos
   */
  void setContinuos(const bool & is_continuos) {
    push([=]{
      is_continuos_ = is_continuos;
    });
  }

  /**
   *
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
      notifyOn(TimerEvents::STARTED);
      timer_.async_wait(std::bind(&Timer::timerExpired, this, std::placeholders::_1));
    });
  }

  /**
   * Method is used to stop waiting timer
   */
  void stop() {
    push([=]{
      notifyOn(TimerEvents::STOPED);
      timer_.cancel();
    });
  }

  /**
   * Unsafe function.
   * User should be confident that _listener lives at moment of calling callback
   * @tparam _Component
   * @param _callback
   * @param _listener
   */
  template <typename _Lisener>
  void connect(void(_Lisener::*_callback)(const TimerEvents &),
               _Lisener * _listener) {
    static_assert(std::is_base_of<IComponent, _Lisener>::value,
                  "_listener is not derived from IComponent");
    if (_listener) {
      unchecked_listeners_->push_back([=](const TimerEvents & _event){
        _listener->push([=]() mutable {
          (_listener->*_callback)(_event);
        });
      });
    }
  }

  /**
   *
   * @tparam _Component
   * @param _callback
   * @param _listener
   */
  template <typename _Lisener>
  void connect(void(_Lisener::*_callback)(const TimerEvents &),
               std::shared_ptr<_Lisener> _listener) {
    static_assert(std::is_base_of<IComponent, _Lisener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerLisener, _Lisener>::value,
                  "_listener is not derived from ITimerLisener");
    if(_listener) {
      checked_listeners_->emplace_back(_listener, [=, pointer = _listener.get()](const TimerEvents & _event) {
        pointer->push([=]() mutable {
          (pointer->*_callback)(_event);
        });
      });
    }
  }

  /**
   *
   * @param _lisener
   */
  template <typename _Lisener>
  void addListener(_Lisener * _listener) {
    static_assert(std::is_base_of<IComponent, _Lisener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerLisener, _Lisener>::value,
                  "_listener is not derived from ITimerLisener");
    if (_listener) {
      unchecked_listeners_->push_back([=](const TimerEvents & _event){
        _listener->push([=]() mutable {
          (_listener->processTimerEvent)(_event);
        });
      });
    }
  }

  /**
   *
   * @param _listener
   */
  template <typename _Lisener>
  void addListener(std::shared_ptr<_Lisener> _listener) {
    static_assert(std::is_base_of<IComponent, _Lisener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<ITimerLisener, _Lisener>::value,
                  "_listener is not derived from ITimerLisener");
    if(_listener) {
      checked_listeners_->emplace_back(_listener, [=, pointer = _listener.get()](const TimerEvents & _event) {
        pointer->push([=]() mutable {
          (pointer->processTimerEvent)(_event);
        });
      });
    }
  }

 protected:
  /**
   *
   * @param _error
   */
  virtual void timerExpired(const boost::system::error_code& _error) {
    if (!_error) {
      notifyOn(TimerEvents::EXPIRED);
      if (is_continuos_) {
        start();
      }
    }
  }

  /**
   * Help method used to notify clients about events
   * @param _event Event regarding which we should notify
   */
  void notifyOn(TimerEvents _event) {
    for (auto & listener : *unchecked_listeners_) {
      listener(_event);
    }
    for (auto & listener : *checked_listeners_) {
      if (auto _observer = listener.first.lock()) {
        listener.second(_event);
      } else {
        // TODO(redra): Delete it
      }
    }
  }

  void exit() override {
    IComponent::exit();
    stop();
  }

 protected:
  bool is_continuos_;
  boost::posix_time::time_duration duration_;
  boost::asio::deadline_timer timer_;

 private:
  std::shared_ptr<tListCallbacks> unchecked_listeners_;
  std::shared_ptr<tCheckedListCallbacks> checked_listeners_;
};

#endif //ICC_TIMER_HPP
