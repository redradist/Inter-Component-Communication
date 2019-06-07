//
// Created by redra on 08.06.19.
//

#include <utility>
#include <icc/os/EventLoop.hpp>
#include <icc/os/posix/EventLoopImpl.hpp>
#include "OSObject.hpp"

namespace icc {

namespace os {

EventLoop & EventLoop::getDefaultInstance() {
  static EventLoop eventLoop{nullptr};
  return eventLoop;
}

std::shared_ptr<EventLoop> EventLoop::createEventLoop() {
  return std::shared_ptr<EventLoop>(new EventLoop());
}

EventLoop::EventLoop()
  : impl_ptr_{new EventLoop::EventLoopImpl} {
}

EventLoop::EventLoop(std::nullptr_t)
  : impl_ptr_{new EventLoop::EventLoopImpl{nullptr}} {
}

EventLoop::~EventLoop() {
  impl_ptr_->stop();
}

void EventLoop::run()  {
  impl_ptr_->run();
}

void EventLoop::stop()  {
  impl_ptr_->stop();
}

std::shared_ptr<IEventLoop::IChannel>
EventLoop::createChannel()  {
  return impl_ptr_->createChannel();
}

std::thread::id
EventLoop::getThreadId() const {
  return impl_ptr_->getThreadId();
}

bool EventLoop::isRun() const {
  return impl_ptr_->isRun();
}

std::shared_ptr<Timer> EventLoop::createTimer() {
  return impl_ptr_->createTimer();
}

void EventLoop::registerObjectEvents(const OSObject & osObject,
                                     const OSObjectEventType & eventType,
                                     function_wrapper<void(const OSObject&)> callback) {
  impl_ptr_->registerObjectEvents(osObject, eventType, std::move(callback));
}

void EventLoop::unregisterObjectEvents(const OSObject & osObject,
                                       const OSObjectEventType & eventType,
                                       function_wrapper<void(const OSObject&)> callback) {
  impl_ptr_->unregisterObjectEvents(osObject, eventType, std::move(callback));
}

}

}