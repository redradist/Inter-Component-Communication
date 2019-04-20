/**
 * @file TaskScheduler.cpp
 * @author Denis Kotov
 * @date 23 Apr 2018
 * @brief Scheduler for starting coroutine Tasks
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#if defined(__cpp_coroutines) && __cpp_coroutines >= 201703

#include "TaskScheduler.hpp"

namespace icc {

namespace coroutine {

TaskScheduler::TaskScheduler()
  : icc::Component(nullptr) {
}

TaskScheduler::TaskScheduler(boost::asio::io_service *_eventLoop)
  : icc::Component(_eventLoop) {
}

TaskScheduler::TaskScheduler(std::shared_ptr<boost::asio::io_service> _eventLoop)
  : icc::Component(_eventLoop) {
}

TaskScheduler::~TaskScheduler() {
}

TaskScheduler &
TaskScheduler::getDefaultTaskSheduler(boost::asio::io_service *_eventLoop) {
  static TaskScheduler sheduler(_eventLoop);
  return sheduler;
}

}

}

#endif
