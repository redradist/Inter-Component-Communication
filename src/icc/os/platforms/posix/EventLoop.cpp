//
// Created by redra on 08.06.19.
//

#include <utility>

#include <icc/os/EventLoop.hpp>
#include <icc/os/timer/Timer.hpp>
#include <icc/os/networking/ServerSocket.hpp>
#include <icc/os/networking/Socket.hpp>

#include "Common.hpp"
#include "EventLoopImpl.hpp"

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

std::shared_ptr<IContext::IChannel>
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
  return std::shared_ptr<Timer>(new Timer(impl_ptr_->createTimerImpl()));
}

std::shared_ptr<ServerSocket> EventLoop::createServerSocket(const std::string _address, const uint16_t _port, const uint16_t _numQueue) {
  return std::shared_ptr<ServerSocket>(new ServerSocket(impl_ptr_->createServerSocketImpl(_address, _port, _numQueue)));
}

std::shared_ptr<Socket> EventLoop::createSocket(const std::string _address, const uint16_t _port) {
  return std::shared_ptr<Socket>(new Socket(impl_ptr_->createSocketImpl(_address, _port)));
}

std::shared_ptr<Socket> EventLoop::createSocket(const Handle & _socketHandle) {
  return std::shared_ptr<Socket>(new Socket(impl_ptr_->createSocketImpl(_socketHandle)));
}

void EventLoop::registerObjectEvents(const Handle & osObject,
                                     const EventType & eventType,
                                     function_wrapper<void(const Handle&)> callback) {
  impl_ptr_->registerObjectEvents(osObject, eventType, std::move(callback));
}

void EventLoop::unregisterObjectEvents(const Handle & osObject,
                                       const EventType & eventType,
                                       function_wrapper<void(const Handle&)> callback) {
  impl_ptr_->unregisterObjectEvents(osObject, eventType, std::move(callback));
}

}

}