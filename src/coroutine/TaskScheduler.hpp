//
// Created by redra on 23.04.18.
//

#ifndef ICC_TASHSHEDULER_HPP
#define ICC_TASHSHEDULER_HPP

#if defined(__cpp_coroutines)
#if __cpp_coroutines >= 201703

#include <boost/asio/io_service.hpp>
#include <IComponent.hpp>
#include "Task.hpp"

namespace icc {

namespace coroutine {

class TaskScheduler
    : public virtual icc::IComponent {
 public:
  TaskScheduler();

  TaskScheduler(boost::asio::io_service *_eventLoop);

  TaskScheduler(std::shared_ptr<boost::asio::io_service> _eventLoop);

  ~TaskScheduler();

  static TaskScheduler & getDefaultTaskSheduler(boost::asio::io_service *_eventLoop);

  template <typename _R>
  void startTask(Task<_R> & _task) {
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
#endif

#endif //ICC_TASHSHEDULER_HPP
