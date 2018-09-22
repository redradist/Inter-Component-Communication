/**
 * @file CommandLoop.cpp
 * @author Denis Kotov
 * @date 08 Jul 2017
 * @brief Contains default Command Loop class which could process
 * any commands or be executed as command by itself
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include <command/exceptions/CommandNotFound.hpp>
#include "CommandLoop.hpp"

namespace icc {

namespace command {

void CommandLoop::processStartCommand() {
  invoke([=] {
    nextCommand();
  });
}

void CommandLoop::processResumeCommand() {
  invoke([=] {
    if (!commands_.empty()) {
      if (!(LoopMode::MultiCommand & mode_)) {
        auto &command = commands_.front();
        if (State::INACTIVE == command->getState()) {
          command->resumeCommand();
        }
      } else {
        for (auto & command : commands_) {
          if (State::INACTIVE == command->getState()) {
            command->resumeCommand();
          }
        }
      }
    }
  });
}

void CommandLoop::processSuspendCommand() {
  invoke([=] {
    if (!commands_.empty()) {
      if (!(LoopMode::MultiCommand & mode_)) {
        auto &command = commands_.front();
        if (State::ACTIVE == command->getState()) {
          command->suspendCommand();
        }
      } else {
        for (auto & command : commands_) {
          if (State::ACTIVE == command->getState()) {
            command->suspendCommand();
          }
        }
      }
    }
  });
}

void CommandLoop::processStopCommand() {
  invoke([=] {
    while (!commands_.empty()) {
      auto &command = commands_.front();
      command->stopCommand();
      commands_.pop_front();
    }
  });
}

void CommandLoop::setMode(LoopMode _mode) {
  invoke([=] {
    if (0 == commands_.size()) {
      mode_ = _mode;
    }
  });
}

void CommandLoop::pushBack(std::shared_ptr<ICommand> _command) {
  invoke([=] {
    auto foundCommand = std::find(commands_.begin(), commands_.end(),
                                  _command);
    if (commands_.end() == foundCommand) {
      commands_.push_back(_command);
    }
    if (State::ACTIVE == getState()) {
      if (1 == commands_.size() || (LoopMode::MultiCommand & mode_)) {
        nextCommand();
      }
    }
  });
}

void CommandLoop::nextCommand() {
  if (!commands_.empty()) {
    if (!(LoopMode::MultiCommand & mode_)) {
      auto &command = commands_.front();
      State commandState = command->getState();
      if (State::ACTIVE != commandState && State::FINISHED != commandState) {
        command->subscribe(std::static_pointer_cast<ICommand::IListener>(
            this->icc::helpers::virtual_enable_shared_from_this<CommandLoop>::shared_from_this()));
        command->startCommand();
      }
    } else {
      for (auto & command : commands_) {
        State commandState = command->getState();
        if (State::ACTIVE != commandState && State::FINISHED != commandState) {
          command->subscribe(std::static_pointer_cast<ICommand::IListener>(
              this->icc::helpers::virtual_enable_shared_from_this<CommandLoop>::shared_from_this()));
          command->startCommand();
        }
      }
    }
  } else if (!(LoopMode::Continuous & mode_)) {
    if (State::ACTIVE == getState()) {
      finished(CommandResult::SUCCESS);
    } else {
      finished(CommandResult::ABORTED);
    }
  }
}

void CommandLoop::processEvent(const CommandData & _data) {
  invoke([=] {
    CommandResult result = _data.result_;
    if (!commands_.empty()) {
      auto foundCommand = std::find(commands_.begin(), commands_.end(),
                                    _data.p_command_);
      if (commands_.end() != foundCommand) {
        commands_.erase(foundCommand);
      }
    }
    if (CommandResult::FAILED == result &&
        !(LoopMode::Continuous & mode_) &&
        (LoopMode::Transaction & mode_)) {
      finished(CommandResult::FAILED);
    } else {
      nextCommand();
    }
  });
}

void CommandLoop::finished(const CommandResult & _result) {
  invoke([=] {
    exit();
    ICommand::finished(_result);
  });
}

int CommandLoop::getCommandType() const {
  return static_cast<int>(CommandTypes::LOOP);
}

void CommandLoop::exit() {
  invoke([=]{
    while (!commands_.empty()) {
      commands_.pop_front();
    }
    Component::exit();
  });
}

std::shared_ptr<ICommand>
CommandLoop::getCommandByIndex(const size_t _index) {
  return getCommandByIndexAsync(_index).get();
}

std::future<std::shared_ptr<ICommand>>
CommandLoop::getCommandByIndexAsync(const size_t _index) {
  auto promise = std::make_shared<std::promise<std::shared_ptr<ICommand>>>();
  auto result = promise->get_future();
  invoke([=]() mutable {
    try {
      if (_index < commands_.size()) {
        promise->set_value(commands_[_index]);
      } else {
        char errorMsg[256];
        sprintf(errorMsg, "Command with _index = %ld is not exist !!", _index);
        throw icc::command::CommandNotFound(errorMsg);
      }
    } catch (...) {
      promise->set_exception(std::current_exception());
    }
  });
  return result;
}

size_t
CommandLoop::getNumberOfCommands() {
  return getNumberOfCommandsAsync().get();
}

std::future<size_t>
CommandLoop::getNumberOfCommandsAsync() {
  auto promise = std::make_shared<std::promise<size_t>>();
  auto result = promise->get_future();
  invoke([=]() mutable {
    promise->set_value(commands_.size());
  });
  return result;
}

std::shared_ptr<ICommand>
CommandLoop::getFirstCommandByType(const int _commandType) {
  return getFirstCommandByTypeAsync(_commandType).get();
}

std::future<std::shared_ptr<ICommand>>
CommandLoop::getFirstCommandByTypeAsync(const int _commandType) {
  auto promise = std::make_shared<std::promise<std::shared_ptr<ICommand>>>();
  auto result = promise->get_future();
  invoke([=]() mutable {
    std::shared_ptr<ICommand> foundCommand;
    auto foundCommandIter = std::find_if(commands_.begin(), commands_.end(),
    [=] (const std::shared_ptr<ICommand> & _command) {
      return _commandType == _command->getCommandType();
    });
    if (commands_.end() != foundCommandIter) {
      foundCommand = *foundCommandIter;
    }
    promise->set_value(foundCommand);
  });
  return result;
}

std::shared_ptr<ICommand>
CommandLoop::getLastCommandByType(const int _commandType) {
  return getLastCommandByTypeAsync(_commandType).get();
}

std::future<std::shared_ptr<ICommand>>
CommandLoop::getLastCommandByTypeAsync(const int _commandType) {
  auto promise = std::make_shared<std::promise<std::shared_ptr<ICommand>>>();
  auto result = promise->get_future();
  invoke([=]() mutable {
    std::shared_ptr<ICommand> foundCommand;
    auto foundCommandIter = std::find_if(commands_.rbegin(), commands_.rend(),
    [=] (const std::shared_ptr<ICommand> & _command) {
      return _commandType == _command->getCommandType();
    });
    if (commands_.rend() != foundCommandIter) {
      foundCommand = *foundCommandIter;
    }
    promise->set_value(foundCommand);
  });
  return result;
}

std::vector<std::shared_ptr<ICommand>>
CommandLoop::findCommandsByType(const int _commandType) {
  return findCommandsByTypeAsync(_commandType).get();
}

std::future<std::vector<std::shared_ptr<ICommand>>>
CommandLoop::findCommandsByTypeAsync(const int _commandType) {
  auto promise = std::make_shared<std::promise<std::vector<std::shared_ptr<ICommand>>>>();
  auto result = promise->get_future();
  invoke([=]() mutable {
    std::vector<std::shared_ptr<ICommand>> foundCommands;
    std::copy_if(commands_.begin(), commands_.end(),
                 std::back_inserter(foundCommands),
    [=] (const std::shared_ptr<ICommand> & _command) {
      return _commandType == _command->getCommandType();
    });
    promise->set_value(foundCommands);
  });
  return result;
}

void CommandLoop::clearLoop() {
  invoke([=]() mutable {
    for (auto & command : commands_) {
      try {
        command->stopCommand();
      } catch(...) {
      }
    }
  });
}

}

}
