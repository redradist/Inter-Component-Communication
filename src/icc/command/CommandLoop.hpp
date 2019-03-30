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

#include <deque>
#include <future>
#include <icc/IComponent.hpp>
#include <icc/Event.hpp>
#include <icc/helpers/memory_helpers.hpp>
#include "ICommandLoop.hpp"

namespace icc {

namespace command {

class CommandLoop
    : public virtual IComponent
    , public ICommandLoop
    , public ICommand::IListener
    , public icc::helpers::virtual_enable_shared_from_this<CommandLoop> {
 protected:
  /**
   * Delegate constructor. Needed for inheritance
   */
  CommandLoop()
      : IComponent(nullptr) {
  }

 private:
  friend class Builder;
  /**
   * Delegate constructor
   */
  CommandLoop(std::nullptr_t)
      : IComponent(nullptr) {
  }

  /**
   * Constructor for initializing within event loop created outside.
   * Owner of this pointer is not we
   * @param _eventLoop Event loop that will be used
   */
  CommandLoop(boost::asio::io_service *_eventLoop)
      : IComponent(_eventLoop) {
  }

  /**
   * Constructor for initializing within event loop created outside
   * @param _eventLoop Event loop that will be used
   */
  CommandLoop(std::shared_ptr<boost::asio::io_service> _eventLoop)
      : IComponent(_eventLoop) {
  }

  /**
   * Used to share event loop of parent object
   * @param _parent Parent compenent that will share event loop
   */
  CommandLoop(IComponent *_parent)
      : IComponent(_parent) {
  }

  /**
   * Used to share event loop of parent object
   * @param _parent Parent compenent that will share event loop
   */
  CommandLoop(std::shared_ptr<IComponent> _parent)
      : IComponent(_parent) {
  }

 public:
  virtual ~CommandLoop() = default;

 public:
  void setMode(LoopMode _mode) override;
  void pushBack(std::shared_ptr<ICommand> _command) override;
  /**
   * Overridden getting command type method
   */
  virtual int getCommandType() const override;

 public:
  size_t getNumberOfCommands();
  std::future<size_t> getNumberOfCommandsAsync();
  std::shared_ptr<ICommand> getCommandByIndex(const size_t _index);
  std::future<std::shared_ptr<ICommand>> getCommandByIndexAsync(const size_t _index);
  std::shared_ptr<ICommand>
  getFirstCommandByType(const int _commandType);
  std::future<std::shared_ptr<ICommand>>
  getFirstCommandByTypeAsync(const int _commandType);
  std::shared_ptr<ICommand>
  getLastCommandByType(const int _commandType);
  std::future<std::shared_ptr<ICommand>>
  getLastCommandByTypeAsync(const int _commandType);
  std::vector<std::shared_ptr<ICommand>>
  findCommandsByType(const int _commandType);
  std::future<std::vector<std::shared_ptr<ICommand>>>
  findCommandsByTypeAsync(const int _commandType);
  void clearLoop();

 protected:
  /**
   * Used to start CommandLoop
   */
  virtual void processStartCommand() override;
  /**
   * Used to resume CommandLoop
   */
  virtual void processResumeCommand() override;
  /**
   * Used to suspend CommandLoop
   */
  virtual void processSuspendCommand() override;
  /**
   * Used to stop CommandLoop
   */
  virtual void processStopCommand() override;
  /**
   * Used to handle events from Commands
   */
  virtual void processEvent(const CommandData & _result) override;
  /**
   * Overridden exit method from ICommand
   */
  virtual void finished(const CommandResult & _result) override;
  /**
   * Launch next command
   */
  virtual void nextCommand();
  /**
   * Overridden exit method from IComponent
   */
  virtual void exit() override;

 protected:
  LoopMode mode_ = LoopMode::Default;
  std::deque<std::shared_ptr<ICommand>> commands_;
};

}

}

#endif //ICC_COMMANDLOOP_HPP
