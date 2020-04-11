#include <utility>

//
// Created by redra on 21.07.17.
//

#include <boost/asio/io_service.hpp>
#include <iostream>
#include <icc/Component.hpp>
#include <icc/Event.hpp>
#include <icc/Timer.hpp>
#include <icc/command/Builder.hpp>
#include <icc/command/Command.hpp>
#include <icc/command/CommandLoop.hpp>
#include <icc/Context.hpp>

namespace icc {

template <>
class Context<boost::asio::io_service>
    : public std::enable_shared_from_this<Context<boost::asio::io_service>>
    , public ContextBase {
 public:
  class Channel : public IContext::IChannel {
   public:
    explicit Channel(std::shared_ptr<Context> context)
        : context_{std::move(context)} {
    }

    void push(Action _action) override {
      if (context_) {
        context_->push(std::move(_action));
      }
    }

    void invoke(Action _action) override {
      if (context_) {
        context_->invoke(std::move(_action));
      }
    }

    [[nodiscard]]
    IContext & getContext() const override {
      return *context_;
    }

   private:
    std::shared_ptr<Context> context_;
  };

  Context(boost::asio::io_service *_service)
    : execute_{true}
    , service_(std::shared_ptr<boost::asio::io_service>(_service,
      [=](boost::asio::io_service *) {
        // NOTE(redra): Nothing need to do. Owner of this pointer is not us
      }))
    , worker_(new boost::asio::io_service::work(*service_)) {
  }

  Context(std::shared_ptr<boost::asio::io_service> _service)
    : execute_{true}
    , service_(std::move(_service))
    , worker_(new boost::asio::io_service::work(*service_)) {
  }

  void push(Action _action) {
    if (execute_.load()) {
      service_->post(std::move(_action));
    }
  }

  void invoke(Action _action) {
    if (execute_.load()) {
      service_->dispatch(_action);
    }
  }

  void run(ExecPolicy _policy = ExecPolicy::Forever) override {
    bool stopState = false;
    if (execute_.compare_exchange_strong(stopState, true)) {
      queue_thread_id_.store(std::this_thread::get_id());
      service_->run();
    }
  }

  void stop() override {
    bool executeState = true;
    if (execute_.compare_exchange_strong(executeState, false)) {
      worker_.reset(nullptr);
    }
  }

  std::unique_ptr<IChannel> createChannel() override {
    return std::unique_ptr<Channel>(new Channel{shared_from_this()});
  }

  std::thread::id getThreadId() const override {
    return queue_thread_id_.load(std::memory_order_acquire);
  }

  bool isRun() const override {
    return execute_.load(std::memory_order_acquire);
  }

 private:
  std::atomic_bool execute_{false};
  std::atomic<std::thread::id> queue_thread_id_;
  std::shared_ptr<boost::asio::io_service> service_;
  std::unique_ptr<boost::asio::io_service::work> worker_;
};

}

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
    : icc::Component(_eventLoop) {
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
    : icc::Component(_eventLoop) {
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
  } catch (icc::command::CommandStateAssert & ex) {
    std::cout << "ex is " << ex.what() << std::endl;
    std::cout << "command is " << ex.getCommand()->getCommandType() << std::endl;
  }
  return 0;
}
