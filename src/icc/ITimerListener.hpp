/**
 * @file TimerLisener.hpp
 * @author Denis Kotov
 * @date 11 Jun 2017
 * @brief Timer listener interface
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_TIMERLISENER_HPP
#define ICC_TIMERLISENER_HPP

#include <icc/Component.hpp>

namespace icc {

enum class TimerEvents {
  STARTED,
  EXPIRED,
  STOPPED,
};

class ITimerListener {
 public:
  virtual void processEvent(const TimerEvents &) = 0;
};

}

#endif //ICC_TIMERLISENER_HPP
