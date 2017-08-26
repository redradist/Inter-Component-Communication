/**
 * @file ICommand.hpp
 * @author Denis Kotov
 * @date 08 Jul 2017
 * @brief Interface class that is used for creation commands
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_ICOMMAND_HPP
#define ICC_ICOMMAND_HPP

#include <IComponent.hpp>
#include "ICommandLisener.hpp"

namespace icc {

namespace command {

class ICommand
  : public icc::helpers::virtual_enable_shared_from_this<ICommand> {
 public:
  using tCallback = void(IComponent::*)(const CommandResult &);

 public:
  ICommand() = default;
  virtual ~ICommand() = 0;

 public:
  /**
   * Used to start CommandLoop
   */
  virtual void startCommand() = 0;
  /**
   * Used to resume CommandLoop
   */
  virtual void resumeCommand() = 0;
  /**
   * Used to suspend CommandLoop
   */
  virtual void suspendCommand() = 0;
  /**
   * Used to stop CommandLoop
   */
  virtual void stopCommand() = 0;

 public:
  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  template<typename _Listener>
  void subscribe(_Listener *_listener) {
    static_assert(std::is_base_of<ICommandListener, _Listener>::value,
                  "_listener is not derived from ICommandListener");
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
    static_assert(std::is_base_of<ICommandListener, _Listener>::value,
                  "_listener is not derived from ICommandListener");
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
    static_assert(std::is_base_of<ICommandListener, _Listener>::value,
                  "_listener is not derived from ICommandListener");
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
    static_assert(std::is_base_of<ICommandListener, _Listener>::value,
                  "_listener is not derived from ICommandListener");
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
    CommandData data;
    data.p_command_ = this->shared_from_this();
    data.result_ = _result;
    event_.operator()(data);
  }

 private:
  Event<void(const CommandData &)> event_;
};

inline
ICommand::~ICommand() {
}

}

}

#endif //ICC_ICOMMAND_HPP
