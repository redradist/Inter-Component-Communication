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
    finished(icc::command::CommandResult::FAILED);
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

  void processEvent(const icc::command::CommandResult & _result) override {
    switch (_result) {
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
    icc::command::CommandLoop::processEvent(_result);
  }
};

class Connect
    : public icc::command::CommandLoop {
 public:
  using icc::IComponent::IComponent;

  void processEvent(const icc::command::CommandResult & _result) override {
    icc::command::CommandLoop::processEvent(_result);
    switch (_result) {
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
    exit();
  }
};

int main() {
  boost::asio::io_service service_;
  std::shared_ptr<Connect> mainLoop = std::make_shared<Connect>(&service_);
  mainLoop->startCommand();
  std::shared_ptr<ConnectionBTProfiles> loop =
      std::make_shared<ConnectionBTProfiles>(&service_);
  loop->setMode(icc::command::CommandLoop::Finite);
  loop->push_back(std::make_shared<ConnectionHFP>());
  loop->push_back(std::make_shared<ConnectionA2DP>());
  mainLoop->push_back(loop);
  // Start event loop
  service_.run();
  return 0;
}
