/**
 * @file Task.hpp
 * @author Denis Kotov
 * @date 18 Apr 2018
 * @brief Suspendable Task (coroutine)
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COROUTINELIBRARY_TASK_HPP
#define COROUTINELIBRARY_TASK_HPP

#if defined(__cpp_coroutines) && __cpp_coroutines >= 201703

#include <type_traits>
#include <iostream>
#include <vector>
#include <future>
#include <memory>
#include <shared_mutex>
#include <condition_variable>
#include <optional>
#include <experimental/coroutine>
#include <icc/Context.hpp>

namespace icc {

namespace coroutine {

class TaskScheduler;

template<typename _R = void>
class TaskPromise;

template<typename _R = void>
class Task;

template<typename _AwaitableType>
class TaskAwaiter {
 public:
  TaskAwaiter(_AwaitableType &&_awaitable)
      : awaitable_(std::move(_awaitable)) {
  }

  TaskAwaiter(TaskAwaiter<_AwaitableType> &&_awaitable)
      : awaitable_(std::move(_awaitable.awaitable_)),
        wait_thread_(std::move(_awaitable.wait_thread_)),
        channel_(std::move(_awaitable.channel_)) {
  }

  ~TaskAwaiter() {
    if (wait_thread_.joinable()) {
      wait_thread_.join();
    }
  }

  bool await_ready() {
    return false;
  }

  auto await_resume() {
    return awaitable_.get();
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    wait_thread_ = std::thread([this, _coro] {
      awaitable_.wait();
      channel_->push([_coro]() mutable {
        _coro.resume();
      });
    });
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

 private:
  _AwaitableType &&awaitable_;
  std::thread wait_thread_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template<typename _R>
class TaskAwaiter<Task<_R>> {
 public:
  TaskAwaiter(Task<_R> &&_awaitable)
      : awaitable_(std::move(_awaitable)) {
  }

  TaskAwaiter(TaskAwaiter<Task<_R>> &&_awaitable)
      : awaitable_(std::move(_awaitable.awaitable_)),
        wait_thread_(std::move(_awaitable.wait_thread_)),
        channel_(std::move(_awaitable.channel_)) {
  }

  ~TaskAwaiter() {
    if (wait_thread_.joinable()) {
      wait_thread_.join();
    }
  }

  bool await_ready() {
    return awaitable_.isReady();
  }

  auto await_resume() {
    return awaitable_.get();
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    wait_thread_ = std::thread([this, _coro] {
      awaitable_.wait();
      channel_->push([_coro]() mutable {
        _coro.resume();
      });
    });
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

 private:
  Task<_R> && awaitable_;
  std::thread wait_thread_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template<typename _R>
class TaskPromise {
 public:
  friend class Task<_R>;

  TaskPromise() = default;
  ~TaskPromise() = default;

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  std::experimental::suspend_always initial_suspend() { return {}; }
  std::experimental::suspend_never final_suspend() { return {}; }
  template<typename _AwaitableType>
  auto await_transform(_AwaitableType &&_result) {
    auto awaiter = TaskAwaiter<_AwaitableType>{std::forward<_AwaitableType>(_result)};
    awaiter.setContextChannel(channel_);
    return awaiter;
  }
  template<typename _F>
  auto await_transform(Task<_F> &&_result) {
    _result.setContextChannel(channel_);
    _result.initialStart();
    auto awaiter = TaskAwaiter<Task<_F>>{std::move(_result)};
    awaiter.setContextChannel(channel_);
    return awaiter;
  }
  template<typename _F>
  auto await_transform(std::optional<_F> &&_result) = delete;
  auto get_return_object() {
    return Task<_R>{*this};
  }
  void return_value(_R value) {
    std::unique_lock<std::mutex> lock(*mutex_);
    *result_ = value;
    awaiter_->notify_one();
  }
  void unhandled_exception() {
    std::rethrow_exception(std::current_exception());
  }

 private:
  std::shared_ptr<std::optional<_R>> result_ = std::make_shared<std::optional<_R>>();
  std::shared_ptr<std::mutex> mutex_ = std::make_shared<std::mutex>();
  std::shared_ptr<std::condition_variable> awaiter_ = std::make_shared<std::condition_variable>();
  std::shared_ptr<IContext::IChannel> channel_;
};

template<typename _R>
class Task {
 public:
  template<typename _U>
  friend class TaskPromise;
  friend class TaskScheduler;
  using HandleType = std::experimental::coroutine_handle<TaskPromise<_R> >;

  Task(const Task<_R> &_request)
      : promise_(_request.promise_),
        result_(_request.result_),
        mutex_(_request.mutex_),
        awaiter_(_request.awaiter_),
        channel_(_request.channel_) {
  }

  Task(Task<_R> &&_request)
      : promise_(_request.promise_),
        result_(std::move(_request.result_)),
        mutex_(std::move(_request.mutex_)),
        awaiter_(std::move(_request.awaiter_)),
        channel_(std::move(_request.channel_)) {
  }

  bool isReady() const {
    return static_cast<bool>(*result_);
  }

  void wait() const {
    std::unique_lock<std::mutex> lock(*mutex_);
    if (!*result_) {
      awaiter_->wait(lock);
    }
  }

  _R get() const {
    std::optional<_R> result;
    if (*result_) {
      result = *result_;
    } else {
      wait();
      result = *result_;
    }
    return result.value();
  }

 protected:
  Task(TaskPromise<_R> &_promise)
      : promise_(_promise), result_(promise_.result_), mutex_(promise_.mutex_), awaiter_(promise_.awaiter_) {
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
    promise_.setContextChannel(channel_);
  }

  void initialStart() {
    if (channel_) {
      channel_->invoke([=] {
        HandleType::from_promise(promise_).resume();
      });
      channel_.reset();
    }
  }

 private:
  TaskPromise<_R> &promise_;
  std::shared_ptr<std::optional<_R>> result_;
  std::shared_ptr<std::mutex> mutex_;
  std::shared_ptr<std::condition_variable> awaiter_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template<>
class TaskPromise<void> {
 public:
  friend class Task<void>;

  TaskPromise() {
  }

  ~TaskPromise() {
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  std::experimental::suspend_always initial_suspend() { return {}; }
  std::experimental::suspend_never final_suspend() { return {}; }
  template<typename _AwaitableType>
  auto await_transform(_AwaitableType &&_result) {
    auto awaiter = TaskAwaiter<_AwaitableType>{std::forward<_AwaitableType>(_result)};
    awaiter.setContextChannel(channel_);
    return awaiter;
  }
  template<typename _F>
  auto await_transform(Task<_F> &&_result) {
    _result.setContextChannel(channel_);
    _result.initialStart();
    auto awaiter = TaskAwaiter<Task<_F>>{std::move(_result)};
    awaiter.setContextChannel(channel_);
    return awaiter;
  }
  template<typename _F>
  auto await_transform(std::optional<_F> &&_result) = delete;
  auto get_return_object();
  void return_void() {
    std::unique_lock<std::mutex> lock(*mutex_);
    *is_ready_ = true;
    awaiter_->notify_one();
  }
  void unhandled_exception() {
    std::rethrow_exception(std::current_exception());
  }

 private:
  std::shared_ptr<bool> is_ready_ = std::make_shared<bool>();
  std::shared_ptr<std::mutex> mutex_ = std::make_shared<std::mutex>();
  std::shared_ptr<std::condition_variable> awaiter_ = std::make_shared<std::condition_variable>();
  std::shared_ptr<IContext::IChannel> channel_;
};

template<>
class Task<void> {
 public:
  template<typename _U>
  friend class TaskPromise;
  friend class TaskScheduler;
  using HandleType = std::experimental::coroutine_handle<TaskPromise<void> >;

  Task(const Task<void> &_request)
      : promise_(_request.promise_),
        is_ready_(promise_.is_ready_),
        mutex_(_request.mutex_),
        awaiter_(_request.awaiter_),
        channel_(_request.channel_) {
  }

  Task(Task<void> &&_request)
      : promise_(_request.promise_),
        is_ready_(promise_.is_ready_),
        mutex_(std::move(_request.mutex_)),
        awaiter_(std::move(_request.awaiter_)),
        channel_(std::move(_request.channel_)) {
  }

  bool isReady() {
    return *is_ready_;
  }

  void wait() const {
    std::unique_lock<std::mutex> lock(*mutex_);
    if (!*is_ready_) {
      awaiter_->wait(lock);
    }
  }

 protected:
  Task(TaskPromise<void> &_promise)
      : promise_(_promise), is_ready_(promise_.is_ready_), mutex_(promise_.mutex_), awaiter_(promise_.awaiter_) {
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
    promise_.setContextChannel(channel_);
  }

  void initialStart() {
    if (channel_) {
      channel_->invoke([=] {
        HandleType::from_promise(promise_).resume();
      });
      channel_.reset();
    }
  }

 private:
  TaskPromise<void> & promise_;
  std::shared_ptr<bool> is_ready_;
  std::shared_ptr<std::mutex> mutex_;
  std::shared_ptr<std::condition_variable> awaiter_;
  std::shared_ptr<IContext::IChannel> channel_;
};

inline
auto TaskPromise<void>::get_return_object() {
  return Task<void>{*this};
}

template<>
class TaskAwaiter<Task<void>> {
 public:
  TaskAwaiter(Task<void> &&_awaitable)
      : awaitable_(std::move(_awaitable)) {
  }

  TaskAwaiter(TaskAwaiter<Task<void>> &&_awaiter)
      : awaitable_(std::move(_awaiter.awaitable_)),
        wait_thread_(std::move(_awaiter.wait_thread_)),
        channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
    if (wait_thread_.joinable()) {
      wait_thread_.join();
    }
  }

  bool await_ready() {
    return awaitable_.isReady();
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    wait_thread_ = std::thread([this, _coro] {
      awaitable_.wait();
      channel_->push([_coro]() mutable {
        _coro.resume();
      });
    });
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

 private:
  Task<void> && awaitable_;
  std::thread wait_thread_;
  std::shared_ptr<IContext::IChannel> channel_;
};

}

}

namespace std::experimental {

template <typename _R, typename... Args>
struct coroutine_traits<icc::coroutine::Task<_R>, Args...> {
  using promise_type = icc::coroutine::TaskPromise<_R>;
};

}

#endif

#endif //COROUTINELIBRARY_TASK_HPP
