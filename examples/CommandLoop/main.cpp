//
// Created by redra on 21.07.17.
//

#include <iostream>
#include <icc/IComponent.hpp>
#include <icc/Event.hpp>
#include <icc/Timer.hpp>
#include <icc/command/Builder.hpp>
#include <icc/command/Command.hpp>
#include <icc/command/CommandLoop.hpp>

class ConnectionHFP
    : public icc::command::Command {
 public:
  virtual void processStartCommand() override {
    std::cout << "ConnectionHFP is started" << std::endl;
    finished(icc::command::CommandResult::FAILED);
  }
};

class ConnectionA2DP
    : public icc::command::Command {
 public:
  virtual void processStartCommand() override {
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
    setMode(icc::command::LoopMode::MultiCommand |
            icc::command::LoopMode::Transaction);
    pushBack(std::make_shared<ConnectionHFP>());
    pushBack(std::make_shared<ConnectionA2DP>());
  }

  virtual void processStartCommand() override {
    std::cout << "ConnectionBTProfiles is started" << std::endl;
    icc::command::CommandLoop::processStartCommand();
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
  }
};

int main() {
  try {
    using icc::command::Builder;
    boost::asio::io_service service_;
    auto mainLoop = Builder::buildCommandLoop<Connect>(&service_);
    mainLoop->pushBack(Builder::buildCommandLoop<ConnectionBTProfiles>(&service_));
    mainLoop->startCommand();
    // Start event loop
    service_.run();
  } catch (icc::command::CommandStateAssert ex) {
    std::cout << "ex is " << ex.what() << std::endl;
    std::cout << "command is " << ex.getCommand()->getCommandType() << std::endl;
  }
  return 0;
}
