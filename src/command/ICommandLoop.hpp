//
// Created by redra on 12.01.18.
//

#ifndef COMMANDLOOP_ICOMMANDLOOP_HPP
#define COMMANDLOOP_ICOMMANDLOOP_HPP

#include <future>
#include "ICommand.hpp"

namespace icc {

namespace command {

enum class LoopState {
  INACTIVE,
  ACTIVE,
  SUSPENDED,
};

enum class LoopMode {
  /**
   * Should be used for setting continuous mode
   */
  Finite,
  /**
   * Should be used for setting one time mode
   */
  Continuous,
};

class ICommandLoop
 : public ICommand {
 public:
  virtual void setMode(LoopMode _mode) = 0;
  virtual void pushBack(std::shared_ptr<ICommand> _command) = 0;
  virtual std::future<LoopState> getState() = 0;
};

}

}

#endif //COMMANDLOOP_ICOMMANDLOOP_HPP
