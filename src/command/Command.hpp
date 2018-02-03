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
  friend class Builder;

 protected:
  using ICommand::ICommand;

 public:
  /**
   * Default implementation of start()
   */
  virtual void startCommand() override {
    is_started = true;
    finished(CommandResult::SUCCESS);
  }

  /**
   * Default implementation of resume()
   */
  virtual void resumeCommand() override {
    is_suspened = false;
    finished(CommandResult::FAILED);
  }

  /**
   * Default implementation of suspend()
   */
  virtual void suspendCommand() override {
    is_suspened = true;
    finished(CommandResult::FAILED);
  }

  /**
   * Default implementation of stop()
   */
  virtual void stopCommand() override {
    is_finished = true;
    finished(CommandResult::ABORTED);
  }

  virtual int getCommandType() override {
    return static_cast<int>(CommandTypes::COMMAND);
  }

//  virtual bool isStarted() const override {
//    return is_started;
//  }
//
//  virtual bool isSuspended() const override {
//    return is_suspened;
//  }
//
//  virtual bool isFinished() const override {
//    return is_finished;
//  }

 private:
  bool is_started = false;
  bool is_suspened = false;
  bool is_finished = false;
};

}

}

#endif //ICC_COMMAND_HPP
