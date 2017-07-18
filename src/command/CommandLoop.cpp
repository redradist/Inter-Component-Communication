//
// Created by redra on 08.07.17.
//

#include "CommandLoop.hpp"

namespace icc {

namespace command {

void CommandLoop::start() {
  send([=] {
    if (LoopState::INACTIVE == state_) {
      state_ = LoopState::ACTIVE;
      nextCommand();
    }
  });
}

void CommandLoop::resume() {
  send([=] {
    if (LoopState::SUSPENDED == state_) {
      state_ = LoopState::ACTIVE;
      if (!commands_.empty()) {
        auto command = commands_.front();
        command->resume();
      }
    }
  });
}

void CommandLoop::suspend() {
  send([=] {
    state_ = LoopState::SUSPENDED;
    if (!commands_.empty()) {
      auto command = commands_.front();
      command->suspend();
    }
  });
}

void CommandLoop::stop() {
  send([=] {
    state_ = LoopState::INACTIVE;
    while (!commands_.empty()) {
      auto command = commands_.front();
      command->stop();
      commands_.pop();
    }
    finished(CommandEvent::ABORTED);
  });
}

void CommandLoop::setMode(LoopMode _mode) {
  send([=] {
    if (LoopState::INACTIVE == state_) {
      mode_ = _mode;
    }
  });
}

void CommandLoop::push_back(std::shared_ptr<ICommand> _command) {
  send([=] {
    commands_.push(_command);
    if (1 == commands_.size()) {
      nextCommand();
    }
  });
}

std::future<LoopState>
CommandLoop::getState() {
  std::shared_ptr<std::promise<LoopState>> promise =
    std::make_shared<std::promise<LoopState>>();
  std::future<LoopState> result = promise->get_future();
  send([=]() mutable {
    promise->set_value(state_);
  });
  return result;
}

void CommandLoop::nextCommand() {
  if (LoopState::ACTIVE == state_) {
    if (!commands_.empty()) {
      auto command = commands_.front();
      command->subscribe(std::static_pointer_cast<ICommandListener>(this->shared_from_this()));
      command->start();
    } else if (Finite == mode_) {
      finished(CommandEvent::SUCCESS);
    }
  }
}

void CommandLoop::processEvent(const CommandEvent & _event) {
  send([=] {
    if (!commands_.empty()) {
      commands_.pop();
    }
    if (Finite == mode_ &&
        CommandEvent::FAILED == _event) {
      finished(CommandEvent::FAILED);
    } else {
      nextCommand();
    }
  });
}

}

}
