//
// Created by redra on 23.04.18.
//

#if defined(__cpp_coroutines)
#if __cpp_coroutines >= 201703

#include "TaskScheduler.hpp"

namespace icc {

namespace coroutine {

TaskScheduler::TaskScheduler()
    : icc::IComponent(nullptr) {
}

TaskScheduler::TaskScheduler(boost::asio::io_service *_eventLoop)
: icc::IComponent(_eventLoop) {
}

TaskScheduler::TaskScheduler(std::shared_ptr<boost::asio::io_service> _eventLoop)
: icc::IComponent(_eventLoop) {
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
#endif
