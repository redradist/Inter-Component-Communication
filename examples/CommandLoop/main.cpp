//
// Created by redra on 21.07.17.
//

#include <iostream>
#include <IComponent.hpp>
#include <Event.hpp>
#include <Timer.hpp>
#include <command/Builder.hpp>
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
 private:
  friend class icc::command::Builder;
  ConnectionBTProfiles(boost::asio::io_service *_eventLoop)
    : icc::IComponent(_eventLoop) {
    setMode(icc::command::LoopMode::Finite);
    pushBack(std::make_shared<ConnectionHFP>());
    pushBack(std::make_shared<ConnectionA2DP>());
  }

 public:
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
 private:
  friend class icc::command::Builder;
  Connect(boost::asio::io_service *_eventLoop)
    : icc::IComponent(_eventLoop) {
  }

 public:
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
    exit();
  }
};

int main() {
  using icc::command::Builder;
  boost::asio::io_service service_;
  auto mainLoop = Builder::buildCommandLoop<Connect>(&service_);
  mainLoop->startCommand();
  mainLoop->pushBack(Builder::buildCommandLoop<ConnectionBTProfiles>(&service_));
  // Start event loop
  service_.run();
  return 0;
}
