//
// Created by redra on 16.07.17.
//

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
   * Method prived by derived class to listen results of command
   */
  virtual void processEvent(const CommandEvent &) = 0;
};

}

}

#endif //FORECAST_ICOMMANDLISENER_HPP
