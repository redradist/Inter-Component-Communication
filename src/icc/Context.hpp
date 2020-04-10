/**
 * @file EventLoop.hpp
 * @author Denis Kotov
 * @date 26 May 2019
 * @brief
 * Contains IContext:
 * It is interface for implementing Context Execution
 * Contains Context<ThreadSafeQueueAction>:
 * It is default implementation of IContext interface
 * based on ThreadSafeQueue. This implementation is used in Component as
 * default Context
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_CONTEXT_HPP
#define ICC_CONTEXT_HPP

#include <atomic>
#include <thread>
#include <functional>
#include <utility>
#include <icc/_private/containers/ThreadSafeQueue.hpp>

namespace icc {

using Action = std::function<void(void)>;

template <typename TService>
class Context;

enum class ExecPolicy {
  Forever,
  UntilWorkers,
};

class IContext {
 public:
  class IChannel {
   public:
    virtual ~IChannel() = 0;
    virtual void push(Action _action) = 0;
    virtual void invoke(Action _action) = 0;
    [[nodiscard]]
    virtual IContext & getContext() const = 0;
  };

  virtual ~IContext() = 0;
  virtual std::unique_ptr<IChannel> createChannel() = 0;
};

inline
IContext::~IContext() {
}

inline
IContext::IChannel::~IChannel() {
}

class ContextBase : public IContext {
 public:
  virtual void run(ExecPolicy _policy = ExecPolicy::Forever) = 0;
  virtual void stop() = 0;
  virtual std::thread::id getThreadId() const = 0;
  virtual bool isRun() const = 0;
};

class ContextBuilder {
 public:
  template <typename TService, typename ... TArgs>
  static std::shared_ptr<ContextBase>
  createContext(TArgs && ... _args) {
    return std::make_shared<Context<TService>>(std::forward<TArgs>(_args)...);
  }

  template <typename TService, typename ... TArgs>
  static std::shared_ptr<ContextBase>
  createContext(TService *_service, TArgs && ... _args) {
    return std::make_shared<Context<TService>>(_service, std::forward<TArgs>(_args)...);
  }

  template <typename TService, typename ... TArgs>
  static std::shared_ptr<ContextBase>
  createContext(std::shared_ptr<TService> _service, TArgs && ... _args) {
    return std::make_shared<Context<TService>>(_service, std::forward<TArgs>(_args)...);
  }

  template <typename TService, typename ... TArgs>
  static std::shared_ptr<ContextBase>
  createContext(std::unique_ptr<TService> _service, TArgs && ... _args) {
    return std::make_shared<Context<TService>>(_service, std::forward<TArgs>(_args)...);
  }
};

using ThreadSafeQueueAction = icc::_private::containers::ThreadSafeQueue<Action>;

template <>
class Context<ThreadSafeQueueAction> final
    : public ContextBase
    , public std::enable_shared_from_this<Context<ThreadSafeQueueAction>> {
 public:
  class Channel : public IContext::IChannel {
   public:
    explicit Channel(std::shared_ptr<Context> context)
      : context_{std::move(context)} {
      context_->num_of_channels_.fetch_add(1, std::memory_order_acq_rel);
    }

    ~Channel() override {
      const uint32_t kPrevNumWorkers = context_->num_of_channels_.fetch_sub(1, std::memory_order_acq_rel);
      if (1 == kPrevNumWorkers) {
        context_->queue_->interrupt();
      }
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

  void run(ExecPolicy _policy = ExecPolicy::Forever) override {
    std::thread::id defaultThreadId;
    if (queue_thread_id_.compare_exchange_strong(defaultThreadId, std::this_thread::get_id())) {
      bool stopState = false;
      if (run_.compare_exchange_strong(stopState, true)) {
        switch (_policy) {
          case ExecPolicy::Forever: {
            runForever();
          }
            break;
          case ExecPolicy::UntilWorkers: {
            runUntilWorkers();
          }
            break;
        }
      }
    }
  }

  void stop() override {
    auto thisThread = std::this_thread::get_id();
    if (queue_thread_id_.compare_exchange_strong(thisThread, std::thread::id())) {
      bool executeState = true;
      if (run_.compare_exchange_strong(executeState, false)) {
        queue_->interrupt();
      }
    }
  }

  std::unique_ptr<IChannel> createChannel() override {
    return std::unique_ptr<Channel>(new Channel{shared_from_this()});
  }

  std::thread::id getThreadId() const override {
    return queue_thread_id_.load(std::memory_order_acquire);
  }

  bool isRun() const override {
    return run_.load(std::memory_order_acquire);
  }

 private:
  void runForever() {
    do {
      Action action = queue_->waitPop();
      if (action) {
        action();
      }
    } while (run_.load(std::memory_order_acquire));
  }

  void runUntilWorkers() {
    do {
      Action action = queue_->waitPop();
      if (!queue_->isInterrupt() && action) {
        action();
      }
    } while (run_.load(std::memory_order_acquire) &&
             num_of_channels_.load(std::memory_order_acquire) > 0);
  }

  std::atomic_bool run_{false};
  std::atomic_uint32_t num_of_channels_{0};
  std::atomic<std::thread::id> queue_thread_id_;
  std::unique_ptr<ThreadSafeQueueAction> queue_{new ThreadSafeQueueAction()};
};

using ThreadSafeQueueContext = Context<ThreadSafeQueueAction>;

}

#endif //ICC_CONTEXT_HPP
