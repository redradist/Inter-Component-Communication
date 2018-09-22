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

#include <boost/asio/io_service.hpp>
#include <IComponent.hpp>
#include "Task.hpp"

namespace icc {

namespace coroutine {

class TaskScheduler
    : public virtual icc::Component {
 public:
  TaskScheduler();

  TaskScheduler(boost::asio::io_service *_eventLoop);

  TaskScheduler(std::shared_ptr<boost::asio::io_service> _eventLoop);

  virtual ~TaskScheduler();

  static TaskScheduler & getDefaultTaskSheduler(boost::asio::io_service *_eventLoop);

  template <typename _R>
  void startCoroutine(Task<_R> & _task) {
    _task.setIOService(getEventLoop());
    _task.initialStart();
  }

  template <typename _R>
  void startCoroutine(Task<_R> &&_task) {
    _task.setIOService(getEventLoop());
    _task.initialStart();
  }
};

}

}

#endif

#endif //ICC_TASHSHEDULER_HPP
