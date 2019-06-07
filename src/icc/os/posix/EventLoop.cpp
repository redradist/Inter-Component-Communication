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
#include <iostream>
#include <algorithm>
#include <memory>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <inttypes.h>
#include <unistd.h>

#include "icc/os/Timer.hpp"
#include "icc/os/EventLoop.hpp"
#include "OSObject.hpp"

namespace icc {

namespace os {

struct EventLoop::InternalEvent {
  OSObject object_;
  icc::_private::helpers::function_wrapper<void(int)> callback_;

  InternalEvent(const OSObject fd, icc::_private::helpers::function_wrapper<void(int)> callback)
      : object_{fd}, callback_{std::move(callback)} {
  }
};

struct EventLoop::ListenerInfo {
  ListenerInfo(const OSObject fd)
      : object_{fd} {
  }

  ListenerInfo(const OSObject fd, std::vector<icc::_private::helpers::function_wrapper<void(int)>> callbacks)
      : object_{fd}, callbacks_{std::move(callbacks)} {
  }

  OSObject object_;
  std::vector<icc::_private::helpers::function_wrapper<void(int)>> callbacks_;
};

EventLoop & EventLoop::getDefaultInstance() {
  static EventLoop eventLoop(nullptr);
  return eventLoop;
}

std::shared_ptr<EventLoop> EventLoop::createEventLoop() {
  return std::shared_ptr<EventLoop>(new EventLoop());
}

EventLoop::EventLoop()
  : event_loop_object_{new EventLoop::OSObject{-1}} {
}

EventLoop::EventLoop(std::nullptr_t)
  : EventLoop() {
  event_loop_thread_ = std::thread(&EventLoop::run, this);
}

EventLoop::~EventLoop() {
  stop();
  if (event_loop_thread_.joinable()) {
    event_loop_thread_.join();
  }
}

std::shared_ptr<Timer> EventLoop::createTimer() {
  const int kTimerFd = timerfd_create(CLOCK_MONOTONIC, 0);
  auto timer = new Timer(EventLoop::OSObject{kTimerFd});
  icc::_private::helpers::function_wrapper<void(int)> callback(
      &Timer::onTimerExpired, timer);
  registerObjectEvents(EventLoop::OSObject{kTimerFd}, ListenerEventType::READ, callback);
  return std::shared_ptr<Timer>(timer,
  [this, callback](Timer* timer) {
    unregisterObjectEvents(*timer->timer_object_, ListenerEventType::READ, callback);
  });
}

std::shared_ptr<IEventLoop::IChannel> EventLoop::createChannel() {
  return nullptr;
}

std::thread::id EventLoop::getThreadId() const {
  return event_loop_thread_id_.load(std::memory_order_acquire);
}

bool EventLoop::isRun() const {
  return execute_.load(std::memory_order_acquire);
}

void EventLoop::run() {
  event_loop_object_->fd_ = eventfd(0, O_NONBLOCK);
  if (event_loop_object_->fd_ == -1) {
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
    maxFd = std::max(maxFd, event_loop_object_->fd_);
    FD_SET(event_loop_object_->fd_, &readFds);
    initListenObjects(read_listeners_, readFds, maxFd);
    initListenObjects(write_listeners_, writeFds, maxFd);
    initListenObjects(error_listeners_, errorFds, maxFd);

    select(maxFd + 1, &readFds, &writeFds, &errorFds, nullptr);
    if (!execute_.load(std::memory_order_acquire)) {
      eventfd_write(event_loop_object_->fd_, 0);
      break;
    }

    handleLoopEvents(readFds);
    handleListenersEvents(read_listeners_, readFds);
    handleListenersEvents(write_listeners_, writeFds);
    handleListenersEvents(error_listeners_, errorFds);
  }
}

void EventLoop::stop() {
  if (event_loop_object_->fd_ != -1) {
    execute_.store(false, std::memory_order_release);
    eventfd_write(event_loop_object_->fd_, 1);
  }
}

void EventLoop::addFdTo(std::lock_guard<std::mutex> &lock,
                        std::vector<ListenerInfo> &listeners,
                        const std::vector<InternalEvent> &addListeners) {
  if (!addListeners.empty()) {
    for (auto &fdInfo : addListeners) {
      auto foundFd = findFdIn(fdInfo.object_, listeners);
      if (foundFd != listeners.end()) {
        foundFd->callbacks_.push_back(fdInfo.callback_);
        auto erased = std::unique(foundFd->callbacks_.begin(), foundFd->callbacks_.end());
        foundFd->callbacks_.erase(erased, foundFd->callbacks_.end());
      } else {
        listeners.emplace_back(fdInfo.object_, std::vector<icc::_private::helpers::function_wrapper<void(int)>>{fdInfo.callback_});
      }
    }
  }
}

void EventLoop::removeFdFrom(std::lock_guard<std::mutex> &lock,
                             std::vector<ListenerInfo> &listeners,
                             const std::vector<InternalEvent> &removeListeners) {
  if (!removeListeners.empty()) {
    for (auto &fdInfo : removeListeners) {
      auto foundFd = findFdIn(fdInfo.object_, listeners);
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

void EventLoop::initListenObjects(std::vector<ListenerInfo> &fds,
                                  fd_set &fdSet,
                                  int &maxFd) const {
  for (const auto &fdInfo : fds) {
    FD_SET(fdInfo.object_.fd_, &fdSet);
    if (fdInfo.object_.fd_ > maxFd) {
      maxFd = fdInfo.object_.fd_;
    }
  }
}

void EventLoop::handleLoopEvents(fd_set fdSet) {
  if (FD_ISSET(event_loop_object_->fd_, &fdSet)) {
    std::lock_guard<std::mutex> lock(internal_mtx_);
    eventfd_t updatedEvent;
    eventfd_read(event_loop_object_->fd_, &updatedEvent);
    if (updatedEvent > 0) {
      addFdTo(lock, read_listeners_, add_read_listeners_);
      addFdTo(lock, write_listeners_, add_write_listeners_);
      addFdTo(lock, error_listeners_, add_error_listeners_);
      removeFdFrom(lock, read_listeners_, remove_read_listeners_);
      removeFdFrom(lock, write_listeners_, remove_write_listeners_);
      removeFdFrom(lock, error_listeners_, remove_error_listeners_);
      eventfd_write(event_loop_object_->fd_, 0);
    }
  }
}

void EventLoop::handleListenersEvents(std::vector<ListenerInfo> &fds, fd_set &fdSet) {
  for (const auto &fdInfo : fds) {
    if (FD_ISSET(fdInfo.object_.fd_, &fdSet)) {
      for (const auto &callback : fdInfo.callbacks_) {
        callback(fdInfo.object_.fd_);
      }
    }
  }
}

std::vector<EventLoop::ListenerInfo>::iterator
EventLoop::findFdIn(const OSObject & osObject, std::vector<ListenerInfo> &fds) {
  auto foundFd = std::find_if(fds.begin(), fds.end(),
                              [osObject](const ListenerInfo &fdInfo) {
                                return fdInfo.object_.fd_ == osObject.fd_;
                              });
  return foundFd;
}

void EventLoop::registerObjectEvents(const OSObject & osObject,
                                     ListenerEventType eventType,
                                     icc::_private::helpers::function_wrapper<void(int)> callback) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  switch (eventType) {
    case READ: {
      add_read_listeners_.emplace_back(osObject, callback);
      break;
    }
    case WRITE: {
      add_write_listeners_.emplace_back(osObject, callback);
      break;
    }
    case ERROR: {
      add_error_listeners_.emplace_back(osObject, callback);
      break;
    }
  }
  if (event_loop_object_->fd_ != -1) {
    eventfd_t updated = 1;
    eventfd_write(event_loop_object_->fd_, updated);
  }
}

void EventLoop::unregisterObjectEvents(const OSObject & osObject,
                                       ListenerEventType eventType,
                                       icc::_private::helpers::function_wrapper<void(int)> callback) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  switch (eventType) {
    case READ: {
      remove_read_listeners_.emplace_back(osObject, callback);
      break;
    }
    case WRITE: {
      remove_write_listeners_.emplace_back(osObject, callback);
      break;
    }
    case ERROR: {
      remove_error_listeners_.emplace_back(osObject, callback);
      break;
    }
  }
  if (event_loop_object_->fd_ != -1) {
    eventfd_t updated = 1;
    eventfd_write(event_loop_object_->fd_, updated);
  }
}

}

}
