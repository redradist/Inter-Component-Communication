/**
 * @file Task.hpp
 * @author Denis Kotov
 * @date 08 Apr 2018
 * @brief Throws when task is invalid
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_TREADPOOL_TASK_HPP
#define ICC_TREADPOOL_TASK_HPP

#include <utility>

#include <icc/Component.hpp>
#include "ThreadPool.hpp"
#include "exceptions/TaskInvalid.hpp"
#include "exceptions/TaskStateAssert.hpp"

namespace icc {

namespace threadpool {

class ThreadPool;

template<typename TRes>
class Task {
 public:
  explicit Task(std::function<TRes(void)> _task) {
    if (_task) {
      task_ = _task;
    } else {
      throw icc::threadpool::TaskInvalid("Main _task is not valid !!");
    }
  }

  void setDescription(const std::string & _description) {
    description_ = _description;
  }

  Task & then(std::function<void(TRes)> _task) {
    if (_task) {
      thens_.push_back(_task);
    } else {
      throw icc::threadpool::TaskInvalid("_task in then is not valid !!");
    }
    return *this;
  }

  template<typename _Component>
  Task & callback(void(_Component::*_callback)(TRes),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<icc::Component, _Component>::value,
                  "_listener is not derived from Component");
    std::weak_ptr<_Component> weakListener = _listener;
    callback_ = [=] (TRes _result) {
      if (auto listener = weakListener.lock()) {
        auto ptrListener = listener.get();
        ptrListener->push([=] {
          (ptrListener->*_callback)(_result);
        });
      }
    };
  }

  template<typename _Component>
  Task & callback(void(_Component::*_callback)(TRes),
                  _Component *_listener) {
    static_assert(std::is_base_of<icc::Component, _Component>::value,
                  "_listener is not derived from Component");
    callback_ = [=] (TRes _result) {
      _listener->push([=] {
        (_listener->*_callback)(_result);
      });
    };
    return *this;
  }

  void start() {
    if (!state_) {
      state_ = true;
      if (thread_pool_ptr_) {
        thread_pool_ptr_->push(static_cast<std::function<void(void)>>(*this));
      } else {
        ThreadPool::getDefaultPool().push(static_cast<std::function<void(void)>>(*this));
      }
    } else {
      throw icc::threadpool::TaskStateAssert("Task is already started !!");
    }
  }

  bool isStarted() const {
    return state_;
  }

  TRes operator()() {
    auto result = task_();
    for (auto & then: thens_) {
      then(result);
    }
    if (callback_) {
      callback_(result);
    }
    return result;
  }

  static void start(std::function<TRes(void)> _task) {
    ThreadPool::getDefaultPool().push(std::move(_task));
  }

 private:
  friend class ThreadPool;

  Task & setThreadPool(std::shared_ptr<ThreadPool> threadPoolPtr) {
    thread_pool_ptr_ = threadPoolPtr;
    return *this;
  }

  bool state_ = false;
  std::string description_ = "";

  std::shared_ptr<ThreadPool> thread_pool_ptr_;
  std::function<TRes(void)> task_ = nullptr;
  std::vector<std::function<void(TRes)>> thens_;
  std::function<void(TRes)> callback_ = nullptr;
};

template<>
class Task<void> {
 public:
  explicit Task(std::function<void(void)> _task) {
    if (_task) {
      task_ = _task;
    } else {
      throw icc::threadpool::TaskInvalid("Main _task is not valid !!");
    }
  }

  void setDescription(const std::string & _description) {
    description_ = _description;
  }

  Task & then(std::function<void(void)> _task) {
    if (_task) {
      thens_.push_back(_task);
    } else {
      throw icc::threadpool::TaskInvalid("_task in then is not valid !!");
    }
    return *this;
  }

  template<typename _Component>
  Task & callback(void(_Component::*_callback)(void),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<icc::Component, _Component>::value,
                  "_listener is not derived from Component");
    std::weak_ptr<_Component> weakListener = _listener;
    callback_ = [=] (void) {
      if (auto listener = weakListener.lock()) {
        auto ptrListener = listener.get();
        ptrListener->push([=] {
          (ptrListener->*_callback)();
        });
      }
    };
    return *this;
  }

  template<typename _Component>
  Task & callback(void(_Component::*_callback)(void),
                  _Component *_listener) {
    static_assert(std::is_base_of<icc::Component, _Component>::value,
                  "_listener is not derived from Component");
    callback_ = [=] () {
      _listener->push([=] {
        (_listener->*_callback)();
      });
    };
    return *this;
  }

  void start() {
    if (!state_) {
      state_ = true;
      if (thread_pool_ptr_) {
        thread_pool_ptr_->push(static_cast<std::function<void(void)>>(*this));
      } else {
        ThreadPool::getDefaultPool().push(static_cast<std::function<void(void)>>(*this));
      }
    } else {
      throw icc::threadpool::TaskStateAssert("Task is already started !!");
    }
  }

  bool isStarted() const {
    return state_;
  }

  void operator()() {
    task_();
    for (auto & then : thens_) {
      then();
    }
    if (callback_) {
      callback_();
    }
  }

  static void start(std::function<void(void)> _task) {
    ThreadPool::getDefaultPool().push(std::move(_task));
  }

 private:
  friend class ThreadPool;

  Task & setThreadPool(std::shared_ptr<ThreadPool> threadPoolPtr) {
    thread_pool_ptr_ = threadPoolPtr;
    return *this;
  }

  bool state_ = false;
  std::string description_ = "";

  std::shared_ptr<ThreadPool> thread_pool_ptr_;
  std::function<void(void)> task_ = nullptr;
  std::vector<std::function<void(void)>> thens_;
  std::function<void(void)> callback_ = nullptr;
};

}

}

#endif //ICC_TREADPOOL_TASK_HPP
