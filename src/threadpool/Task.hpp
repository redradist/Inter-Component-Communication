/**
 * @file Task.hpp
 * @author Denis Kotov
 * @date 08 Apr 2018
 * @brief Throws when task is invalid
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_TREADPOOL_TASK_HPP
#define ICC_TREADPOOL_TASK_HPP

#include <Component.hpp>
#include "ThreadPool.hpp"
#include "exceptions/TaskInvalid.hpp"
#include "exceptions/TaskStateAssert.hpp"

namespace icc {

namespace pools {

template<typename _R>
class Task {
 public:
  Task(std::function<_R(void)> _task) {
    if (_task) {
      task_ = _task;
    } else {
      throw icc::pools::TaskInvalid("Main _task is not valid !!");
    }
  }

  void setDescription(const std::string & _description) {
    description_ = _description;
  }

  Task &then(std::function<void(_R)> _task) {
    if (_task) {
      thens_.push_back(_task);
    } else {
      throw icc::pools::TaskInvalid("_task in then is not valid !!");
    }
    return *this;
  }

  template<typename _Component>
  Task & callback(void(_Component::*_callback)(_R),
                  std::shared_ptr<_Component> _listener) {
    static_assert(std::is_base_of<icc::IComponent, _Component>::value,
                  "_listener is not derived from Component");
    std::weak_ptr<_Component> weakListener = _listener;
    callback_ = [=] (_R _result) {
      if (auto listener = weakListener.lock()) {
        auto ptrListener = listener.get();
        ptrListener->push([=] {
          (ptrListener->*_callback)(_result);
        });
      }
    };
  }

  template<typename _Component>
  Task & callback(void(_Component::*_callback)(_R),
                  _Component *_listener) {
    static_assert(std::is_base_of<icc::IComponent, _Component>::value,
                  "_listener is not derived from Component");
    callback_ = [=] (_R _result) {
      _listener->push([=] {
        (_listener->*_callback)(_result);
      });
    };
    return *this;
  }

  void start() {
    if (false == state_) {
      state_ = true;
      ThreadPool::getPool().push(static_cast<std::function<void(void)>>(*this));
    } else {
      throw icc::pools::TaskStateAssert("Task is already started !!");
    }
  }

  bool isTaskStarted() const {
    return state_;
  }

  void operator()() {
    auto result = task_();
    for (auto & then: thens_) {
      then(result);
    }
    if (callback_) {
      callback_(result);
    }
  }

  operator std::function<void(void)>() {
    auto task = *this;
    return [task]() mutable {
      task.operator()();
    };
  }

  static void start(std::function<void(void)> _task) {
    ThreadPool::getPool().push(_task);
  }

 private:
  bool state_ = false;
  std::string description_ = "";

  std::function<_R(void)> task_ = nullptr;
  std::vector<std::function<void(_R)>> thens_;
  std::function<void(_R)> callback_ = nullptr;
};

}

}

#endif //ICC_TREADPOOL_TASK_HPP
