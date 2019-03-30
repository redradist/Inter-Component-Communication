/**
 * @file ICommand.hpp
 * @author Denis Kotov
 * @date 08 Jul 2017
 * @brief Interface class that is used for creation commands
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_ICOMMAND_HPP
#define ICC_ICOMMAND_HPP

#include <memory>
#include <icc/IComponent.hpp>
#include "ICommandData.hpp"
#include "ICommandHandler.hpp"
#include "State.hpp"
#include "icc/command/exceptions/CommandStateAssert.hpp"

namespace icc {

namespace command {

enum class CommandResult {
  SUCCESS,
  FAILED,
  ABORTED,
};

enum class CommandTypes : int {
  LOOP = -1,   // Default value for command loops
  COMMAND = 0, // Default value for commands
  // ... Other user-defined command types
};

class ICommand
  : public ICommandData
  , public ICommandHandler
  , public icc::helpers::virtual_enable_shared_from_this<ICommand> {
 public:

  struct CommandData {
    std::shared_ptr<ICommand> p_command_;
    CommandResult             result_;
  };

  class IListener : public virtual IComponent {
   public:
    IListener() : IComponent(nullptr) {}
    /**
     * Method proved by derived class to listen results of command
     */
    virtual void processEvent(const CommandData &) = 0;
  };

 protected:
  ICommand() {
    state_.store(State::INACTIVE);
  }

 public:
  virtual ~ICommand() = 0;

 public:
  /**
   * Used to start CommandLoop
   */
  virtual void startCommand() final {
    State expected = State::INACTIVE;
    if (state_.compare_exchange_strong(expected, State::ACTIVE)) {
      processStartCommand();
    } else {
      throw icc::command::CommandStateAssert{shared_from_this(),
                                             "State of command is not INACTIVE"};
    }
  }

  /**
   * Used to resume CommandLoop
   */
  virtual void resumeCommand() final {
    State expected = State::SUSPENDED;
    if (state_.compare_exchange_strong(expected, State::ACTIVE)) {
      processResumeCommand();
    } else {
      throw icc::command::CommandStateAssert{shared_from_this(),
                                             "State of command is not SUSPENDED"};
    }
  }

  /**
   * Used to suspend CommandLoop
   */
  virtual void suspendCommand() final {
    State expected = State::ACTIVE;
    if (state_.compare_exchange_strong(expected, State::SUSPENDED)) {
      processSuspendCommand();
    } else {
      throw icc::command::CommandStateAssert{shared_from_this(),
                                             "State of command is not ACTIVE"};
    }
  }

  /**
   * Used to stop CommandLoop
   */
  virtual void stopCommand() final {
    State expected = State::ACTIVE;
    if (state_.compare_exchange_strong(expected, State::INACTIVE)) {
      processStopCommand();
    } else {
      throw icc::command::CommandStateAssert{shared_from_this(),
                                             "State of command is not ACTIVE"};
    }
  }

 public:
  /**
   * Sections that described Command Meta Data
   */
  State getState() const final {
    return state_;
  }

 public:
  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template<typename _Listener>
  void subscribe(_Listener *_listener) {
    static_assert(std::is_base_of<IListener, _Listener>::value,
                  "_listener is not derived from ICommand::IListener");
    if (_listener) {
      event_.connect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template<typename _Listener>
  void subscribe(std::shared_ptr<_Listener> _listener) {
    static_assert(std::is_base_of<IListener, _Listener>::value,
                  "_listener is not derived from ICommand::IListener");
    if (_listener) {
      event_.connect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  template<typename _Listener>
  void unsubscribe(_Listener *_listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<IListener, _Listener>::value,
                  "_listener is not derived from ICommand::IListener");
    if (_listener) {
      event_.disconnect(&_Listener::processEvent, _listener);
    }
  }

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  template<typename _Listener>
  void unsubscribe(std::shared_ptr<_Listener> _listener) {
    static_assert(std::is_base_of<IComponent, _Listener>::value,
                  "_listener is not derived from IComponent");
    static_assert(std::is_base_of<IListener, _Listener>::value,
                  "_listener is not derived from ICommand::IListener");
    if (_listener) {
      event_.disconnect(&_Listener::processEvent, _listener);
    }
  }

 protected:
  /**
   * Method to finish command
   * @param _result Result with which command is finished
   */
  virtual void finished(const CommandResult & _result) {
    State expected = State::ACTIVE;
    if (state_.compare_exchange_strong(expected, State::FINISHED)) {
      event_.operator()({shared_from_this(), _result});
    }
  }

 private:
  std::atomic<State> state_;
  Event<void(const CommandData &)> event_;
};

inline
ICommand::~ICommand() {
}

}

}

#endif //ICC_ICOMMAND_HPP
