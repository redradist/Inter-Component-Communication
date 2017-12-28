//
// Created by redra on 21.07.17.
//

#include <iostream>
#include <IComponent.hpp>
#include <Event.hpp>
#include <Timer.hpp>
#include <command/Command.hpp>
#include <command/CommandLoop.hpp>

class ConnectionHFP
    : public icc::command::Command {
 public:
  virtual void startCommand() override {
    std::cout << "ConnectionHFP is started" << std::endl;
    finished(icc::command::CommandResult::SUCCESS);
  }
};

class ConnectionA2DP
    : public icc::command::Command {
 public:
  virtual void startCommand() override {
    std::cout << "ConnectionA2DP is started" << std::endl;
    finished(icc::command::CommandResult::SUCCESS);
  }
};

class ConnectionBTProfiles
  : public icc::command::CommandLoop {
 public:
  using icc::IComponent::IComponent;
  ConnectionBTProfiles(boost::asio::io_service *_eventLoop)
    : icc::IComponent(_eventLoop) {
    setMode(icc::command::LoopMode::Finite);
    push_back(std::make_shared<ConnectionHFP>());
    push_back(std::make_shared<ConnectionA2DP>());
  }

  void processEvent(const CommandData & _data) override {
    icc::command::CommandLoop::processEvent(_data);
    switch (_data.result_) {
      case icc::command::CommandResult::SUCCESS:
        std::cout << "ConnectionBTProfiles::CommandEvent::SUCCESS" << std::endl;
        break;
      case icc::command::CommandResult::FAILED:
        std::cout << "ConnectionBTProfiles::CommandEvent::FAILED" << std::endl;
        break;
      case icc::command::CommandResult::ABORTED:
        std::cout << "ConnectionBTProfiles::CommandEvent::ABORTED" << std::endl;
        break;
    }
  }
};

class Connect
    : public icc::command::CommandLoop {
 public:
  using icc::IComponent::IComponent;

  void processEvent(const CommandData & _data) override {
    icc::command::CommandLoop::processEvent(_data);
    switch (_data.result_) {
      case icc::command::CommandResult::SUCCESS:
        std::cout << "Connect::CommandEvent::SUCCESS" << std::endl;
        break;
      case icc::command::CommandResult::FAILED:
        std::cout << "Connect::CommandEvent::FAILED" << std::endl;
        break;
      case icc::command::CommandResult::ABORTED:
        std::cout << "Connect::CommandEvent::ABORTED" << std::endl;
        break;
    }
    std::cout << "Connect::Command is finished" << std::endl;
  }
};

int main() {
  boost::asio::io_service service_;
  std::shared_ptr<Connect> mainLoop = std::make_shared<Connect>(&service_);
  mainLoop->startCommand();
  mainLoop->push_back(std::make_shared<ConnectionBTProfiles>(&service_));
  // Start event loop
  service_.run();
  return 0;
}
