/**
 * @file CommandLoop.cpp
 * @author Denis Kotov
 * @date 08 Jul 2017
 * @brief Contains default Command Loop class which could process
 * any commands or be executed as command by itself
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include "CommandLoop.hpp"

namespace icc {

namespace command {

void CommandLoop::startCommand() {
  send([=]{
    if (LoopState::INACTIVE == state_) {
      state_ = LoopState::ACTIVE;
      nextCommand();
    }
  });
}

void CommandLoop::resumeCommand() {
  send([=]{
    if (LoopState::SUSPENDED == state_) {
      state_ = LoopState::ACTIVE;
      if (!commands_.empty()) {
        auto & command = commands_.front();
        command->resumeCommand();
      }
    }
  });
}

void CommandLoop::suspendCommand() {
  send([=]{
    state_ = LoopState::SUSPENDED;
    if (!commands_.empty()) {
      auto & command = commands_.front();
      command->suspendCommand();
    }
  });
}

void CommandLoop::stopCommand() {
  send([=]{
    state_ = LoopState::INACTIVE;
    while (!commands_.empty()) {
      auto & command = commands_.front();
      command->stopCommand();
      commands_.pop();
    }
    finished(CommandResult::ABORTED);
  });
}

void CommandLoop::setMode(LoopMode _mode) {
  send([=]{
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
      auto & command = commands_.front();
      command->subscribe(std::static_pointer_cast<ICommandListener>(this->shared_from_this()));
      command->startCommand();
    } else if (Finite == mode_) {
      finished(CommandResult::SUCCESS);
    }
  }
}

void CommandLoop::processEvent(const CommandResult & _result) {
  send([=]{
    if (!commands_.empty()) {
      commands_.pop();
    }
    if (Finite == mode_ &&
        CommandResult::FAILED == _result) {
      finished(CommandResult::FAILED);
    } else {
      nextCommand();
    }
  });
}

void CommandLoop::finished(const CommandResult & _result) {
  ICommand::finished(_result);
  send([=]{
    exit();
  });
}

void CommandLoop::exit() {
  push([=]{
    while (!commands_.empty()) {
      commands_.pop();
    }
    IComponent::exit();
  });
}

}

}
