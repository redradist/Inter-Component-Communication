/**
 * @file TimerLisener.hpp
 * @author Denis Kotov
 * @date 11 Jun 2017
 * @brief Timer listener interface
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_OS_TIMERLISENER_HPP
#define ICC_OS_TIMERLISENER_HPP

namespace icc {

namespace os {

class ITimerListener {
 public:
  virtual void onTimerExpired() = 0;
};

}

}

#endif //ICC_OS_TIMERLISENER_HPP
