//
// Created by redra on 08.07.17.
//

#ifndef ICC_ICOMMAND_HPP
#define ICC_ICOMMAND_HPP

#include <IComponent.hpp>
#include "ICommandLisener.hpp"

namespace icc {

namespace command {

class ICommand {
 public:
  using tCallback = void(IComponent::*)(const CommandEvent &);

 public:
  ICommand() = default;
  virtual ~ICommand() = 0;

 public:
  /**
   * Used to start CommandLoop
   */
  virtual void start() = 0;
  /**
   * Used to resume CommandLoop
   */
  virtual void resume() = 0;
  /**
   * Used to suspend CommandLoop
   */
  virtual void suspend() = 0;
  /**
   * Used to stop CommandLoop
   */
  virtual void stop() = 0;

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
  virtual void finished(const CommandEvent & _event) {
    event_.operator()(_event);
  }

 private:
  std::weak_ptr<IComponent> p_loop_;
  std::function<void(const CommandEvent &)> callback_;
  Event<void(const CommandEvent &)> event_;
};

inline
ICommand::~ICommand() {
}

}

}

#endif //ICC_ICOMMAND_HPP