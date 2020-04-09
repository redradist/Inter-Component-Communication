/**
 * @file TaskScheduler.hpp
 * @author Denis Kotov
 * @date 23 Apr 2018
 * @brief Scheduler for starting coroutine Tasks
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_TASHSHEDULER_HPP
#define ICC_TASHSHEDULER_HPP

#if defined(__cpp_coroutines) && __cpp_coroutines >= 201703

#include <icc/Component.hpp>
#include "Task.hpp"

namespace icc {

namespace coroutine {

class TaskScheduler
    : public virtual icc::Component {
 public:
  TaskScheduler();

  template <typename TService>
  TaskScheduler(TService * _contextChannel)
      : icc::Component(_contextChannel) {
  }

  TaskScheduler(std::shared_ptr<IContext::IChannel> _contextChannel);

  virtual ~TaskScheduler();

  static TaskScheduler & getDefaultTaskSheduler(std::shared_ptr<IContext::IChannel> _contextChannel);

  template <typename _R>
  void startCoroutine(Task<_R> & _task) {
    _task.setContextChannel(getChannel());
    _task.initialStart();
  }

  template <typename _R>
  void startCoroutine(Task<_R> &&_task) {
    _task.setContextChannel(getChannel());
    _task.initialStart();
  }
};

}

}

#endif

#endif //ICC_TASHSHEDULER_HPP
