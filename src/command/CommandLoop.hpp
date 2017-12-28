/**
 * @file CommandLoop.hpp
 * @author Denis Kotov
 * @date 08 Jul 2017
 * @brief Contains default Command Loop class which could process
 * any commands or be executed as command by itself
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_COMMANDLOOP_HPP
#define ICC_COMMANDLOOP_HPP

#include <queue>
#include <future>
#include <IComponent.hpp>
#include <Event.hpp>
#include <helpers/memory_helpers.hpp>
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

class CommandLoop
    : public icc::helpers::virtual_enable_shared_from_this<CommandLoop>,
      public virtual IComponent,
      public ICommand,
      public ICommand::IListener {
 public:
  /**
   * Delegate constructors from IComponent
   */
  using IComponent::IComponent;

  CommandLoop() = default;
  virtual ~CommandLoop() = default;

 public:
  /**
   * Used to start CommandLoop
   */
  virtual void startCommand() override;
  /**
   * Used to resume CommandLoop
   */
  virtual void resumeCommand() override;
  /**
   * Used to suspend CommandLoop
   */
  virtual void suspendCommand() override;
  /**
   * Used to stop CommandLoop
   */
  virtual void stopCommand() override;

  /**
   * Overridden exit method from ICommand
   */
  virtual void finished(const CommandResult & _result) override;

  /**
   * Overridden getting command type method
   */
  virtual int getCommandType() override;

  /**
   * Overridden exit method from IComponent
   */
  virtual void exit() override;

 public:
  virtual void setMode(LoopMode _mode);
  virtual void push_back(std::shared_ptr<ICommand> _command);

  std::future<LoopState> getState();

 protected:
  virtual void processEvent(const CommandData & _result) override;
  virtual void nextCommand();

 protected:
  LoopMode mode_ = LoopMode::Continuous;
  LoopState state_ = LoopState::INACTIVE;
  std::queue<std::shared_ptr<ICommand>> commands_;
};

}

}

#endif //ICC_COMMANDLOOP_HPP
