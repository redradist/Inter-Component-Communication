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

  bool push_back(std::shared_ptr<ICommand>);

  virtual void startCommand();
  virtual void abortCommand();

  virtual void receiveNotifications(const CommandResult &) = 0;

 protected:
  void finished(CommandResult);

 protected:
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
