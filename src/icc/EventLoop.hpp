//
// Created by redra on 26.05.19.
//

#ifndef ICC_EVENTLOOP_HPP
#define ICC_EVENTLOOP_HPP

#include <atomic>
#include <thread>
#include <functional>

#include <icc/_private/helpers/thread_safe_queue.hpp>
#include "IChannel.hpp"

namespace icc {

using Action = std::function<void(void)>;

class IEventLoop : public IChannel {
 public:
  virtual void exec() = 0;
  virtual void stop() = 0;
  virtual std::thread::id getThreadId() const = 0;
  virtual bool isExec() const = 0;
};

template <typename TQueue>
class EventLoop;

using TThreadSafeQueue = icc::_private::containers::ThreadSafeQueue<Action>;

template <>
class EventLoop<TThreadSafeQueue> : public IEventLoop {
 public:
  void push(Action _action) override {
    if (execute_.load()) {
      queue_->push(std::move(_action));
    }
  }

  void invoke(Action _action) override {
    if (execute_.load()) {
      if (queue_thread_id_.load() == std::this_thread::get_id()) {
        _action();
      } else {
        queue_->push(std::move(_action));
      }
    }
  }

  void exec() override {
    bool stopExecute = false;
    if (execute_.compare_exchange_strong(stopExecute, true)) {
      while (true) {
        Action action = std::move(queue_->waitPop());
        if (!execute_.load()) {
          break;
        }
        action();
      }
    }
  }

  void stop() override {
    bool startExecute = true;
    if (execute_.compare_exchange_strong(startExecute, false)) {
      queue_->stop();
    }
  }

  std::thread::id getThreadId() const override {
    return queue_thread_id_.load(std::memory_order_acquire);
  }

  bool isExec() const override {
    return execute_.load(std::memory_order_acquire);
  }

 private:
  std::atomic_bool execute_{false};
  std::atomic<std::thread::id> queue_thread_id_;
  std::unique_ptr<TThreadSafeQueue> queue_{new TThreadSafeQueue()};
};

}

#endif //ICC_EVENTLOOP_HPP
