/**
 * @file ICommand.hpp
 * @author Denis Kotov
 * @date 16 Jun 2017
 * @brief Interface class that is used for listening for command
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef FORECAST_ICOMMANDLISENER_HPP
#define FORECAST_ICOMMANDLISENER_HPP

#include "ICommand.hpp"

namespace icc {

namespace command {

enum class CommandEvent {
  SUCCESS,
  FAILED,
  ABORTED,
};

class ICommandListener
  : public virtual IComponent {
 public:
  /**
   * Method proved by derived class to listen results of command
   */
  virtual void processEvent(const CommandEvent &) = 0;
};

}

}

#endif //FORECAST_ICOMMANDLISENER_HPP
