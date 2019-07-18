/**
 * @file EventLoop.cpp
 * @author Denis Kotov
 * @date 1 Jun 2019
 * @brief
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <algorithm>
#include <memory>


#include <inttypes.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/fcntl.h>

#include "EventLoopImpl.hpp"
#include "TimerImpl.hpp"

namespace icc {

namespace os {

EventLoop::EventLoopImpl::EventLoopImpl(std::nullptr_t)
  : EventLoopImpl() {
  event_loop_thread_ = std::thread(&EventLoop::EventLoopImpl::run, this);
}

EventLoop::EventLoopImpl::~EventLoopImpl() {
  stop();
  if (event_loop_thread_.joinable()) {
    event_loop_thread_.join();
  }
}

std::shared_ptr<Timer::TimerImpl> EventLoop::EventLoopImpl::createTimerImpl() {
  const int kTimerFd = timerfd_create(CLOCK_MONOTONIC, 0);
  auto timer = new Timer::TimerImpl(OSObject{kTimerFd});
  function_wrapper<void(const OSObject&)> callback(&Timer::TimerImpl::onTimerExpired, timer);
  registerObjectEvents(OSObject{kTimerFd}, OSObjectEventType::READ, callback);
  return std::shared_ptr<Timer::TimerImpl>(timer,
  [this, callback](Timer::TimerImpl* timer) {
    unregisterObjectEvents(timer->timer_object_, OSObjectEventType::READ, callback);
  });
}

std::shared_ptr<IContext::IChannel> EventLoop::EventLoopImpl::createChannel() {
  return nullptr;
}

std::thread::id EventLoop::EventLoopImpl::getThreadId() const {
  return event_loop_thread_id_.load(std::memory_order_acquire);
}

bool EventLoop::EventLoopImpl::isRun() const {
  return execute_.load(std::memory_order_acquire);
}

void EventLoop::EventLoopImpl::run() {
  event_loop_object_.fd_ = eventfd(0, O_NONBLOCK);
  if (event_loop_object_.fd_ == -1) {
    std::cerr << strerror(errno) << "\n";
    throw "Error !!";
  }
  event_loop_thread_id_.store(std::this_thread::get_id(), std::memory_order_release);
  execute_.store(true, std::memory_order_release);
  {
    std::lock_guard<std::mutex> lock(internal_mtx_);
    addFdTo(lock, read_listeners_, add_read_listeners_);
    addFdTo(lock, write_listeners_, add_write_listeners_);
    addFdTo(lock, error_listeners_, add_error_listeners_);
    removeFdFrom(lock, read_listeners_, remove_read_listeners_);
    removeFdFrom(lock, write_listeners_, remove_write_listeners_);
    removeFdFrom(lock, error_listeners_, remove_error_listeners_);
  }
  while (execute_.load(std::memory_order_acquire)) {
    fd_set readFds;
    fd_set writeFds;
    fd_set errorFds;

    int maxFd = 0;
    maxFd = std::max(maxFd, event_loop_object_.fd_);
    FD_SET(event_loop_object_.fd_, &readFds);
    initFds(read_listeners_, readFds, maxFd);
    initFds(write_listeners_, writeFds, maxFd);
    initFds(error_listeners_, errorFds, maxFd);

    select(maxFd + 1, &readFds, &writeFds, &errorFds, nullptr);
    if (!execute_.load(std::memory_order_acquire)) {
      eventfd_write(event_loop_object_.fd_, 0);
      break;
    }

    handleLoopEvents(readFds);
    handleOSObjectsEvents(read_listeners_, readFds);
    handleOSObjectsEvents(write_listeners_, writeFds);
    handleOSObjectsEvents(error_listeners_, errorFds);
  }
}

void EventLoop::EventLoopImpl::stop() {
  if (event_loop_object_.fd_ != -1) {
    execute_.store(false, std::memory_order_release);
    eventfd_write(event_loop_object_.fd_, 1);
  }
}

void EventLoop::EventLoopImpl::registerObjectEvents(const OSObject & osObject,
                                     const OSObjectEventType & eventType,
                                     function_wrapper<void(const OSObject&)> callback) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  switch (eventType) {
    case OSObjectEventType::READ: {
      add_read_listeners_.emplace_back(osObject, callback);
      break;
    }
    case OSObjectEventType::WRITE: {
      add_write_listeners_.emplace_back(osObject, callback);
      break;
    }
    case OSObjectEventType::ERROR: {
      add_error_listeners_.emplace_back(osObject, callback);
      break;
    }
  }
  eventfd_t updated = 1;
  eventfd_write(event_loop_object_.fd_, updated);
}

void EventLoop::EventLoopImpl::unregisterObjectEvents(const OSObject & osObject,
                                       const OSObjectEventType & eventType,
                                       function_wrapper<void(const OSObject&)> callback) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  switch (eventType) {
    case OSObjectEventType::READ: {
      remove_read_listeners_.emplace_back(osObject, callback);
      break;
    }
    case OSObjectEventType::WRITE: {
      remove_write_listeners_.emplace_back(osObject, callback);
      break;
    }
    case OSObjectEventType::ERROR: {
      remove_error_listeners_.emplace_back(osObject, callback);
      break;
    }
  }
  eventfd_t updated = 1;
  eventfd_write(event_loop_object_.fd_, updated);
}

void EventLoop::EventLoopImpl::addFdTo(std::lock_guard<std::mutex> &lock,
                        std::vector<OSObjectListeners> &listeners,
                        const std::vector<InternalEvent> &addListeners) {
  if (!addListeners.empty()) {
    for (auto &fdInfo : addListeners) {
      auto foundFd = findOSObjectIn(fdInfo.object_, listeners);
      if (foundFd != listeners.end()) {
        foundFd->callbacks_.push_back(fdInfo.callback_);
        auto erased = std::unique(foundFd->callbacks_.begin(), foundFd->callbacks_.end());
        foundFd->callbacks_.erase(erased, foundFd->callbacks_.end());
      } else {
        listeners.emplace_back(fdInfo.object_, std::vector<function_wrapper<void(const OSObject &)>>{fdInfo.callback_});
      }
    }
  }
}

void EventLoop::EventLoopImpl::removeFdFrom(std::lock_guard<std::mutex> &lock,
                             std::vector<OSObjectListeners> &listeners,
                             const std::vector<InternalEvent> &removeListeners) {
  if (!removeListeners.empty()) {
    for (auto &fdInfo : removeListeners) {
      auto foundFd = findOSObjectIn(fdInfo.object_, listeners);
      if (foundFd != listeners.end()) {
        auto itemToRemove = std::remove(foundFd->callbacks_.begin(), foundFd->callbacks_.end(), fdInfo.callback_);
        foundFd->callbacks_.erase(itemToRemove);
        if (foundFd->callbacks_.empty()) {
          listeners.erase(foundFd);
        }
      }
    }
  }
}

void EventLoop::EventLoopImpl::initFds(std::vector<OSObjectListeners> &fds,
                                       fd_set &fdSet,
                                       int &maxFd) const {
  for (const auto &fdInfo : fds) {
    FD_SET(fdInfo.object_.fd_, &fdSet);
    if (fdInfo.object_.fd_ > maxFd) {
      maxFd = fdInfo.object_.fd_;
    }
  }
}

void EventLoop::EventLoopImpl::handleLoopEvents(fd_set fdSet) {
  if (FD_ISSET(event_loop_object_.fd_, &fdSet)) {
    std::lock_guard<std::mutex> lock(internal_mtx_);
    eventfd_t updatedEvent;
    eventfd_read(event_loop_object_.fd_, &updatedEvent);
    if (updatedEvent > 0) {
      addFdTo(lock, read_listeners_, add_read_listeners_);
      addFdTo(lock, write_listeners_, add_write_listeners_);
      addFdTo(lock, error_listeners_, add_error_listeners_);
      removeFdFrom(lock, read_listeners_, remove_read_listeners_);
      removeFdFrom(lock, write_listeners_, remove_write_listeners_);
      removeFdFrom(lock, error_listeners_, remove_error_listeners_);
      eventfd_write(event_loop_object_.fd_, 0);
    }
  }
}

void EventLoop::EventLoopImpl::handleOSObjectsEvents(std::vector<OSObjectListeners> &fds, fd_set &fdSet) {
  for (const auto &fdInfo : fds) {
    if (FD_ISSET(fdInfo.object_.fd_, &fdSet)) {
      for (const auto &callback : fdInfo.callbacks_) {
        callback(fdInfo.object_);
      }
    }
  }
}

std::vector<EventLoop::EventLoopImpl::OSObjectListeners>::iterator
EventLoop::EventLoopImpl::findOSObjectIn(const OSObject &osObject, std::vector<OSObjectListeners> &fds) {
  auto foundFd = std::find_if(fds.begin(), fds.end(),
                              [osObject](const OSObjectListeners &fdInfo) {
                                return fdInfo.object_.fd_ == osObject.fd_;
                              });
  return foundFd;
}

}

}
