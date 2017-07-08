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
  using tCallback = void(IComponent::*)(const CommandResult &);

 public:
  virtual void startCommand() = 0;
  virtual void abortCommand() = 0;

 protected:
  virtual void finished(const CommandResult & _result) {
    push([=]{
      if (auto loop = p_loop_.lock()) {
        p_loop_->push([=]{
          (loop.get()->*_callback)(_result);
        });
      }
    });
  }

 private:
  void setLoop(void(IComponent::*_callback)(const CommandResult &),
               std::shared_ptr<IComponent> _loop) {
    push([=]{
      p_loop_ = _loop;
      callback_ = _callback;
    });
  }

 private:
  friend class ICommandLoop;
  tCallback callback_;
  std::weak_ptr<IComponent> p_loop_;
};

}

}

#endif //ICC_ICOMMAND_HPP
