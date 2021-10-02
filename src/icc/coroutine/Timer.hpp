/**
 * @file Timer.hpp
 * @author Denis Kotov
 * @date 25 Apr 2018
 * @brief Suspendable Timer implementation (coroutine)
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COROUTINELIBRARY_TIMER_HPP
#define COROUTINELIBRARY_TIMER_HPP

#if defined(__cpp_impl_coroutine)

#include <coroutine>

#if defined(__cpp_lib_coroutine)

#include <type_traits>
#include <iostream>
#include <vector>
#include <future>
#include <shared_mutex>
#include <condition_variable>
#include <optional>
#if __has_include(<boost/date_time/posix_time/posix_time.hpp>)
#include <boost/date_time/posix_time/posix_time.hpp>
#endif
#include <icc/os/timer/ITimerListener.hpp>
#include <icc/os/timer/Timer.hpp>
#include <icc/os/EventLoop.hpp>

namespace icc {

namespace coroutine {

#if __has_include(<boost/date_time/posix_time/posix_time.hpp>)
template <>
class TaskAwaiter<boost::posix_time::time_duration>
 : public icc::os::ITimerListener {
 public:
  TaskAwaiter(boost::posix_time::time_duration && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::time_duration> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_.total_nanoseconds()));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  boost::posix_time::time_duration duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<boost::posix_time::hours>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(boost::posix_time::hours && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::hours> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::hours(duration_.total_seconds() / 3600));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  boost::posix_time::hours duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<boost::posix_time::minutes>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(boost::posix_time::minutes && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::minutes> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_.total_nanoseconds()));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  boost::posix_time::minutes duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<boost::posix_time::seconds>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(boost::posix_time::seconds && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::seconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_.total_nanoseconds()));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  boost::posix_time::seconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<boost::posix_time::milliseconds>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(boost::posix_time::milliseconds && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::milliseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_.total_nanoseconds()));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  boost::posix_time::milliseconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<boost::posix_time::microseconds>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(boost::posix_time::microseconds && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::microseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_.total_nanoseconds()));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  boost::posix_time::microseconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS

template <>
class TaskAwaiter<boost::posix_time::nanoseconds>
  : public icc::os::ITimerListener {
 public:
  TaskAwaiter(boost::posix_time::nanoseconds && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::nanoseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_.total_nanoseconds()));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  boost::posix_time::nanoseconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

#endif

#endif

template <class _Rep, class _Period>
class TaskAwaiter<std::chrono::duration<_Rep, _Period>>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(std::chrono::duration<_Rep, _Period> && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::duration<_Rep, _Period>> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  std::chrono::seconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<std::chrono::hours>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(std::chrono::hours && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::hours> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  std::chrono::seconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<std::chrono::minutes>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(std::chrono::minutes && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::minutes> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  std::chrono::seconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<std::chrono::seconds>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(std::chrono::seconds && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::seconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  std::chrono::seconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<std::chrono::milliseconds>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(std::chrono::milliseconds && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::milliseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  std::chrono::milliseconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<std::chrono::microseconds>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(std::chrono::microseconds && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::microseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  std::chrono::microseconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

template <>
class TaskAwaiter<std::chrono::nanoseconds>
    : public icc::os::ITimerListener {
 public:
  TaskAwaiter(std::chrono::nanoseconds && _duration)
      : duration_(_duration)
      , timer_(icc::os::EventLoop::getDefaultInstance().createTimer()) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::nanoseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_))
      , channel_(std::move(_awaiter.channel_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->setInterval(std::chrono::nanoseconds(duration_));
    timer_->addListener(this);
    timer_->start();
  }

  void setContextChannel(std::shared_ptr<IContext::IChannel> _contextChannel) {
    channel_ = _contextChannel;
  }

  void onTimerExpired() override {
    channel_->push([=] {
      coro_.resume();
    });
  }

 private:
  std::coroutine_handle<> coro_;
  std::chrono::nanoseconds duration_;
  std::shared_ptr<icc::os::Timer> timer_;
  std::shared_ptr<IContext::IChannel> channel_;
};

}

}

#endif

#endif

#endif //COROUTINELIBRARY_TIMER_HPP
