/**
 * @file TimerLisener.hpp
 * @author Denis Kotov
 * @date 11 Jun 2017
 * @brief Timer lisener class
 * @copyright MIT License. Open source: https://github.com/redradist/Transport_Buffers.git
 */

#ifndef ICC_TIMERLISENER_HPP
#define ICC_TIMERLISENER_HPP

#include "EventLoopProvider.hpp"

enum class TimerEvents {
  STARTED,
  EXPIRED,
  STOPED,
};

class ITimerLisener
  : public IEventLoopProvider {
 public:
  virtual void processTimerEvent(const TimerEvents &) = 0;
};

#endif //ICC_TIMERLISENER_HPP
