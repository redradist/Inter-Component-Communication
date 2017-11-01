/**
 * @file Command.hpp
 * @author Denis Kotov
 * @date 21 Jul 2017
 * @brief Default Command class which just stub virtual methods
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_COMMAND_HPP
#define ICC_COMMAND_HPP

#include "ICommand.hpp"

namespace icc {

namespace command {

class Command
  : public ICommand {
 public:
  /**
   * Default implementation of start()
   */
  virtual void startCommand() override {
    finished(CommandResult::SUCCESS);
  }

  /**
   * Default implementation of resume()
   */
  virtual void resumeCommand() override {
    finished(CommandResult::FAILED);
  }

  /**
   * Default implementation of suspend()
   */
  virtual void suspendCommand() override {
    finished(CommandResult::FAILED);
  }

  /**
   * Default implementation of stop()
   */
  virtual void stopCommand() override {
    finished(CommandResult::ABORTED);
  }

  virtual int getCommandType() override {
    return static_cast<int>(CommandTypes::COMMAND);
  }

};

}

}

#endif //ICC_COMMAND_HPP
