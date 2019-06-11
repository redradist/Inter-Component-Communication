/**
 * @file EventLoop.hpp
 * @author Denis Kotov
 * @date 26 May 2019
 * @brief
 * Contains IEventLoop:
 * It is interface for implementing Event Loops
 * Contains EventLoop<ThreadSafeQueueAction>:
 * It is default implementation of IEventLoop interface
 * based on ThreadSafeQueue. This implementation is used in Component as
 * default EventLoop
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_EVENTLOOP_HPP
#define ICC_EVENTLOOP_HPP

#include <atomic>
#include <thread>
#include <functional>
#include <utility>
#include <icc/_private/containers/ThreadSafeQueue.hpp>

namespace icc {

using Action = std::function<void(void)>;

template <typename TService>
class EventLoop;

class IEventLoop {
 public:
  template <typename TService>
  static std::shared_ptr<IEventLoop>
  createEventLoop() {
    return std::make_shared<EventLoop<TService>>();
  }

  template <typename TService>
  static std::shared_ptr<IEventLoop>
  createEventLoop(TService *_service) {
    return std::make_shared<EventLoop<TService>>(_service);
  }

  template <typename TService>
  static std::shared_ptr<IEventLoop>
  createEventLoop(std::shared_ptr<TService> _service) {
    return std::make_shared<EventLoop<TService>>(_service);
  }

  template <typename TService>
  static std::shared_ptr<IEventLoop>
  createEventLoop(std::unique_ptr<TService> _service) {
    return std::make_shared<EventLoop<TService>>(_service);
  }

  class IChannel {
   public:
    virtual void push(Action _action) = 0;
    virtual void invoke(Action _action) = 0;
  };

  virtual void run() = 0;
  virtual void stop() = 0;
  virtual std::shared_ptr<IChannel> createChannel() = 0;
  virtual std::thread::id getThreadId() const = 0;
  virtual bool isRun() const = 0;
};

using ThreadSafeQueueAction = icc::_private::containers::ThreadSafeQueue<Action>;

template <>
class EventLoop<ThreadSafeQueueAction> final
    : public std::enable_shared_from_this<EventLoop<ThreadSafeQueueAction>>
    , public IEventLoop {
 public:
  class Channel : public IEventLoop::IChannel {
   public:
    Channel(std::shared_ptr<EventLoop> eventLoop)
      : event_loop_{std::move(eventLoop)} {
    }

    void push(Action _action) override {
      if (event_loop_) {
        event_loop_->push(std::move(_action));
      }
    }

    void invoke(Action _action) override {
      if (event_loop_) {
        event_loop_->invoke(std::move(_action));
      }
    }

   private:
    std::shared_ptr<EventLoop> event_loop_;
  };

  void push(Action _action) {
    queue_->push(std::move(_action));
  }

  void invoke(Action _action) {
    if (queue_thread_id_.load(std::memory_order_acquire) ==
        std::this_thread::get_id()) {
      _action();
    } else {
      queue_->push(std::move(_action));
    }
  }

  void run() override {
    bool stopState = false;
    if (run_.compare_exchange_strong(stopState, true)) {
      queue_->reset();
       do {
        Action action = queue_->waitPop();
        if (action) {
          action();
        }
      } while (run_.load(std::memory_order_acquire));
    }
  }

  void stop() override {
    bool executeState = true;
    if (run_.compare_exchange_strong(executeState, false)) {
      queue_->interrupt();
    }
  }

  std::shared_ptr<IChannel> createChannel() override {
    return std::make_shared<Channel>(shared_from_this());
  }

  std::thread::id getThreadId() const override {
    return queue_thread_id_.load(std::memory_order_acquire);
  }

  bool isRun() const override {
    return run_.load(std::memory_order_acquire);
  }

 private:
  std::atomic_bool run_{false};
  std::atomic<std::thread::id> queue_thread_id_;
  std::unique_ptr<ThreadSafeQueueAction> queue_{new ThreadSafeQueueAction()};
};

}

#endif //ICC_EVENTLOOP_HPP
