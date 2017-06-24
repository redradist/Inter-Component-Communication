/**
 * @file TimerLisener.hpp
 * @author Denis Kotov
 * @date 11 Jun 2017
 * @brief Timer listener interface
 * @copyright MIT License. Open source:
 */

#ifndef ICC_TIMERLISENER_HPP
#define ICC_TIMERLISENER_HPP

#include "IComponent.hpp"

enum class TimerEvents {
  STARTED,
  EXPIRED,
  STOPED,
};

class ITimerLisener {
 public:
  virtual void processEvent(const TimerEvents &) = 0;
};

#endif //ICC_TIMERLISENER_HPP
