//
// Created by redra on 08.07.17.
//

#ifndef ICC_ICOMMAND_HPP
#define ICC_ICOMMAND_HPP

#include <IComponent.hpp>

class ICommandLoop;

namespace icc {

namespace command {

enum class CommandResult {
  SUCCESS,
  FAILED,
  ABORTED,
};

class ICommand
    : public IComponent {
 public:

  virtual void startCommand() = 0;
  virtual void abortCommand() = 0;

 protected:
  void finished(const CommandResult &);

 private:
  friend class ICommandLoop;
  std::shared_ptr<IComponent> p_loop_;
};

void ICommand::finished(CommandResult) {
  p_loop_->push([=]() {

  });
}

}

}

#endif //ICC_ICOMMAND_HPP
