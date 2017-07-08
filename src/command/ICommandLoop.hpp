//
// Created by redra on 08.07.17.
//

#ifndef ICC_ICOMMANDLOOP_HPP
#define ICC_ICOMMANDLOOP_HPP

#include <IComponent.hpp>
#include "ICommand.hpp"

namespace icc {

namespace command {

class ICommandLoop
    : public IComponent {
 public:
  ICommandLoop() = default;
  ~ICommandLoop() = default;

  void push_back(std::shared_ptr<ICommand> _command) {
    push([=]{
      commands_.push(_command);
    });
  }

  virtual void start();
  virtual void nextCommand() {
    push([=]{
      if (!commands_.empty()) {
        auto command = commands_.front();
        command->setLoop(this);
        command->start();
      }
    });
  }

  virtual void abortCommand() {

  }

  virtual void receiveNotifications(const CommandResult &) = 0;

 protected:
  virtual void onFinishCommand(const CommandResult & _result) {
    push([=]{
      this->receiveNotifications(_result);
      if (!commands_.empty()) {
        commands_.pop();
      }
      nextCommand();
    });
  }

 protected:
  bool is_active_command_ = false;
  std::queue<std::shared_ptr<ICommand>> commands_;
};

inline
void ICommandLoop::startCommand() {

}

inline
void ICommandLoop::abortCommand() {

}

}

}

#endif //ICC_ICOMMANDLOOP_HPP
