/**
 * @file TaskScheduler.cpp
 * @author Denis Kotov
 * @date 23 Apr 2018
 * @brief Scheduler for starting coroutine Tasks
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */


#include "TaskScheduler.hpp"

#if defined(__cpp_lib_coroutine)

namespace icc {

namespace coroutine {

TaskScheduler::TaskScheduler()
  : icc::Component(nullptr) {
}

TaskScheduler::TaskScheduler(std::unique_ptr<IContext::IChannel> _contextChannel)
  : icc::Component(std::move(_contextChannel)) {
}

TaskScheduler::~TaskScheduler() {
}

TaskScheduler &
TaskScheduler::getDefaultTaskSheduler(std::unique_ptr<IContext::IChannel> _contextChannel) {
  static TaskScheduler sheduler(std::move(_contextChannel));
  return sheduler;
}

}

}

#endif
