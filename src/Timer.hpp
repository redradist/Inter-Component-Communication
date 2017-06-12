/**
 * @file Timer.hpp
 * @author Denis Kotov
 * @date 10 Jun 2017
 * @brief Timer class
 * @copyright MIT License. Open source: https://github.com/redradist/Transport_Buffers.git
 */

#ifndef ICC_TIMER_HPP
#define ICC_TIMER_HPP

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Component.hpp"
#include "TimerLisener.hpp"

class Timer {
 public:
  Timer(IComponent * _parent)
    : timer_(*_parent->getEventLoop()) {
  }

  void setContinuos(const bool & is_continuos) {
    timer_.get_io_service().post([=]{
      is_continuos_ = is_continuos;
    });
  }

  void start() {

  }

  void addListener(ITimerLisener * _lisener) {
    timer_.get_io_service().post([=]{
      _lisener->getEventLoop()->post([=] {

      });
    });
  }

 protected:
  bool is_continuos_;
  boost::asio::deadline_timer timer_;
};

#endif //ICC_TIMER_HPP
