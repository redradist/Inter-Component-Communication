/**
 * @file CommandLoop.hpp
 * @author Denis Kotov
 * @date 12 Jan 2018
 * @brief Contains interface ICommandLoop
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMANDLOOP_ICOMMANDLOOP_HPP
#define COMMANDLOOP_ICOMMANDLOOP_HPP

#include <future>
#include "ICommand.hpp"
#include "LoopMode.hpp"
#include "State.hpp"

namespace icc {

namespace command {

class ICommandLoop
 : public ICommand {
 public:
  virtual void setMode(LoopMode _mode) = 0;
  virtual void pushBack(std::shared_ptr<ICommand> _command) = 0;
};

}

}

#endif //COMMANDLOOP_ICOMMANDLOOP_HPP
