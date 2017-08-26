/**
 * @file ICommand.hpp
 * @author Denis Kotov
 * @date 16 Jul 2017
 * @brief Interface class that is used for listening for command
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef FORECAST_ICOMMANDLISENER_HPP
#define FORECAST_ICOMMANDLISENER_HPP

#include <memory>
#include "ICommand.hpp"

namespace icc {

namespace command {

enum class CommandResult {
  SUCCESS,
  FAILED,
  ABORTED,
};

struct CommandData {
  std::weak_ptr<ICommand> p_command_;
  CommandResult           result_;
};

class ICommandListener
  : public virtual IComponent {
 public:
  /**
   * Method proved by derived class to listen results of command
   */
  virtual void processEvent(const CommandData &) = 0;
};

}

}

#endif //FORECAST_ICOMMANDLISENER_HPP
