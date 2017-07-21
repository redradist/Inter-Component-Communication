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
    finished(icc::command::CommandEvent::FAILED);
  }
};

class ConnectionA2DP
    : public icc::command::Command {
 public:
  virtual void startCommand() override {
    std::cout << "ConnectionA2DP is started" << std::endl;
    finished(icc::command::CommandEvent::SUCCESS);
  }
};

class ConnectionBTProfiles
  : public icc::command::CommandLoop {
 public:
  using icc::IComponent::IComponent;

  void processEvent(const icc::command::CommandEvent & _event) override {
    switch (_event) {
      case icc::command::CommandEvent::SUCCESS:
        std::cout << "ConnectionBTProfiles::CommandEvent::SUCCESS" << std::endl;
        break;
      case icc::command::CommandEvent::FAILED:
        std::cout << "ConnectionBTProfiles::CommandEvent::FAILED" << std::endl;
        break;
      case icc::command::CommandEvent::ABORTED:
        std::cout << "ConnectionBTProfiles::CommandEvent::ABORTED" << std::endl;
        break;
    }
    icc::command::CommandLoop::processEvent(_event);
  }
};

class Connect
    : public icc::command::CommandLoop {
 public:
  using icc::IComponent::IComponent;

  void processEvent(const icc::command::CommandEvent & _event) override {
    icc::command::CommandLoop::processEvent(_event);
    switch (_event) {
      case icc::command::CommandEvent::SUCCESS:
        std::cout << "Connect::CommandEvent::SUCCESS" << std::endl;
        break;
      case icc::command::CommandEvent::FAILED:
        std::cout << "Connect::CommandEvent::FAILED" << std::endl;
        break;
      case icc::command::CommandEvent::ABORTED:
        std::cout << "Connect::CommandEvent::ABORTED" << std::endl;
        break;
    }
    std::cout << "Connect::Command is finished" << std::endl;
    exit();
  }
};

int main() {
  boost::asio::io_service service_;
  std::shared_ptr<Connect> mainLoop =
      std::make_shared<Connect>(&service_);
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
