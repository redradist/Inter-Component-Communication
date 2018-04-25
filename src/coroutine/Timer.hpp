/**
 * @file Timer.hpp
 * @author Denis Kotov
 * @date 25 Apr 2018
 * @brief Suspendable Timer implementation (coroutine)
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COROUTINELIBRARY_TIMER_HPP
#define COROUTINELIBRARY_TIMER_HPP

#if defined(__cpp_coroutines) && __cpp_coroutines >= 201703

#include <type_traits>
#include <iostream>
#include <vector>
#include <future>
#include <shared_mutex>
#include <condition_variable>
#include <boost/optional.hpp>
#include <boost/asio/io_service.hpp>
#include <experimental/coroutine>

namespace icc {

namespace coroutine {

template <>
class TaskAwaiter<boost::posix_time::time_duration> {
 public:
  TaskAwaiter(boost::posix_time::time_duration && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::time_duration> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(duration_);
    timer_->async_wait(std::bind(&TaskAwaiter<boost::posix_time::time_duration>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  boost::posix_time::time_duration duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<boost::posix_time::hours> {
 public:
  TaskAwaiter(boost::posix_time::hours && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::hours> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(duration_);
    timer_->async_wait(std::bind(&TaskAwaiter<boost::posix_time::hours>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  boost::posix_time::hours duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<boost::posix_time::minutes> {
 public:
  TaskAwaiter(boost::posix_time::minutes && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::minutes> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(duration_);
    timer_->async_wait(std::bind(&TaskAwaiter<boost::posix_time::minutes>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  boost::posix_time::minutes duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<boost::posix_time::seconds> {
 public:
  TaskAwaiter(boost::posix_time::seconds && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::seconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(duration_);
    timer_->async_wait(std::bind(&TaskAwaiter<boost::posix_time::seconds>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  boost::posix_time::seconds duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<boost::posix_time::milliseconds> {
 public:
  TaskAwaiter(boost::posix_time::milliseconds && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::milliseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(duration_);
    timer_->async_wait(std::bind(&TaskAwaiter<boost::posix_time::milliseconds>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  boost::posix_time::milliseconds duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<boost::posix_time::microseconds> {
 public:
  TaskAwaiter(boost::posix_time::microseconds && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::microseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(duration_);
    timer_->async_wait(std::bind(&TaskAwaiter<boost::posix_time::microseconds>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  boost::posix_time::microseconds duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS

template <>
class TaskAwaiter<boost::posix_time::nanoseconds> {
 public:
  TaskAwaiter(boost::posix_time::nanoseconds && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<boost::posix_time::nanoseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(duration_);
    timer_->async_wait(std::bind(&TaskAwaiter<boost::posix_time::nanoseconds>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  boost::posix_time::nanoseconds duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

#endif

template <class _Rep, class _Period>
class TaskAwaiter<std::chrono::duration<_Rep, _Period>> {
 public:
  TaskAwaiter(std::chrono::duration<_Rep, _Period> && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::duration<_Rep, _Period>> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;

    typedef std::chrono::nanoseconds duration_t;
    typedef duration_t::rep rep_t;
    auto d = std::chrono::duration_cast<duration_t>(duration_).count();
    auto sec = d / 1000000000;
    auto nsec = d % 1000000000;
    auto boostTime = boost::posix_time::from_time_t(0) +
        boost::posix_time::seconds(static_cast<long>(sec)) +
#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
        boost::posix_time::nanoseconds(nsec);
#else
        boost::posix_time::microseconds((nsec + 500) / 1000);
#endif
    timer_->expires_from_now(boost::posix_time::time_duration(boostTime - boost::posix_time::from_time_t(0)));
    timer_->async_wait(std::bind(&TaskAwaiter<std::chrono::duration<_Rep, _Period>>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  std::chrono::duration<_Rep, _Period> duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<std::chrono::hours> {
 public:
  TaskAwaiter(std::chrono::hours && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::hours> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(boost::posix_time::hours(duration_.count()));
    timer_->async_wait(std::bind(&TaskAwaiter<std::chrono::hours>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  std::chrono::hours duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<std::chrono::minutes> {
 public:
  TaskAwaiter(std::chrono::minutes && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::minutes> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(boost::posix_time::minutes(duration_.count()));
    timer_->async_wait(std::bind(&TaskAwaiter<std::chrono::minutes>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  std::chrono::minutes duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<std::chrono::seconds> {
 public:
  TaskAwaiter(std::chrono::seconds && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::seconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(boost::posix_time::seconds(duration_.count()));
    timer_->async_wait(std::bind(&TaskAwaiter<std::chrono::seconds>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  std::chrono::seconds duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<std::chrono::milliseconds> {
 public:
  TaskAwaiter(std::chrono::milliseconds && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::milliseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(boost::posix_time::milliseconds(duration_.count()));
    timer_->async_wait(std::bind(&TaskAwaiter<std::chrono::milliseconds>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  std::chrono::milliseconds duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

template <>
class TaskAwaiter<std::chrono::microseconds> {
 public:
  TaskAwaiter(std::chrono::microseconds && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::microseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(boost::posix_time::microseconds(duration_.count()));
    timer_->async_wait(std::bind(&TaskAwaiter<std::chrono::microseconds>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  std::chrono::microseconds duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS

template <>
class TaskAwaiter<std::chrono::nanoseconds> {
 public:
  TaskAwaiter(std::chrono::nanoseconds && _duration)
      : duration_(_duration) {
  }

  TaskAwaiter(TaskAwaiter<std::chrono::nanoseconds> && _awaiter)
      : duration_(std::move(_awaiter.duration_))
      , timer_(std::move(_awaiter.timer_)) {
  }

  ~TaskAwaiter() {
  }

  bool await_ready() {
    return false;
  }

  void await_resume() {
  }

  void await_suspend(std::experimental::coroutine_handle<> _coro) {
    coro_ = _coro;
    timer_->expires_from_now(boost::posix_time::nanoseconds(duration_.count()));
    timer_->async_wait(std::bind(&TaskAwaiter<std::chrono::nanoseconds>::timerExpired,
                                 this, std::placeholders::_1));
  }

  void setIOService(boost::asio::io_service &_service) {
    timer_ = std::make_unique<boost::asio::deadline_timer>(_service);
  }

  void timerExpired(const boost::system::error_code &_error) {
    if (!_error) {
      coro_.resume();
    }
  }

 private:
  std::experimental::coroutine_handle<> coro_;
  std::chrono::nanoseconds duration_;
  std::unique_ptr<boost::asio::deadline_timer> timer_;
};

#endif

}

}

#endif

#endif //COROUTINELIBRARY_TIMER_HPP
