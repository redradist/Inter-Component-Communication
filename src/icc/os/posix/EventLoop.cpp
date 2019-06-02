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

#include "EventLoop.hpp"

namespace icc {

namespace os {

namespace posix {

struct EventLoop::InternalEvent {
  int fd_;
  void (*callback_)(int);

  InternalEvent(const int fd, void(*callback)(int))
      : fd_{fd}, callback_{std::move(callback)} {
  }
};

struct EventLoop::FdInfo {
  FdInfo(const int fd)
      : fd_{fd} {
  }

  FdInfo(const int fd, std::vector<void (*)(int)> callbacks)
      : fd_{fd}, callbacks_{std::move(callbacks)} {
  }

  int fd_;
  std::vector<void (*)(int)> callbacks_;
};

EventLoop & EventLoop::getDefaultInstance() {
  static EventLoop eventLoop;
  return eventLoop;
}

EventLoop::EventLoop(std::nullptr_t) {
  event_loop_thread_ = std::thread(&EventLoop::run, this);
}

EventLoop::~EventLoop() {
  if (event_loop_thread_.joinable()) {
    event_loop_thread_.join();
  }
}

std::shared_ptr<EventLoop> EventLoop::createInstance() {
  return std::shared_ptr<EventLoop>(new EventLoop());
}

std::shared_ptr<IEventLoop::IChannel> EventLoop::createChannel() {
  return nullptr;
}

std::thread::id EventLoop::getThreadId() const {

}

bool EventLoop::isRun() const {
  return execute_.load(std::memory_order_acquire);
}

void EventLoop::run() {
  event_loop_fd_ = eventfd(0, O_NONBLOCK);
  if (event_loop_fd_ == -1) {
    std::cerr << strerror(errno) << "\n";
    // throw
    return;
  }
  execute_.store(true, std::memory_order_release);
  while (execute_.load(std::memory_order_acquire)) {
    fd_set readFds;
    fd_set writeFds;
    fd_set errorFds;

    int maxFd = 0;
    maxFd = std::max(maxFd, event_loop_fd_);
    FD_SET(event_loop_fd_, &readFds);
    initFds(read_fds_, readFds, maxFd);
    initFds(write_fds_, writeFds, maxFd);
    initFds(error_fds_, errorFds, maxFd);

    select(maxFd + 1, &readFds, &writeFds, &errorFds, nullptr);
    if (!execute_.load(std::memory_order_acquire)) {
      eventfd_write(event_loop_fd_, 0);
      break;
    }

    handleLoopEvents(readFds);
    handleFdsEvents(read_fds_, readFds);
    handleFdsEvents(write_fds_, writeFds);
    handleFdsEvents(error_fds_, errorFds);
  }
}

void EventLoop::stop() {
  execute_.store(false, std::memory_order_release);
  eventfd_write(event_loop_fd_, 1);
}

void EventLoop::addFdTo(std::lock_guard<std::mutex> &lock,
                        std::vector<FdInfo> &fds,
                        const std::vector<InternalEvent> &addFds) {
  if (!addFds.empty()) {
    for (auto &fdInfo : addFds) {
      auto foundFd = findFdIn(fdInfo.fd_, fds);
      if (foundFd != fds.end()) {
        foundFd->callbacks_.push_back(fdInfo.callback_);
        auto erased = std::unique(foundFd->callbacks_.begin(), foundFd->callbacks_.end());
        foundFd->callbacks_.erase(erased, foundFd->callbacks_.end());
      } else {
        fds.emplace_back(fdInfo.fd_, std::vector<void (*)(int)>{fdInfo.callback_});
      }
    }
  }
}

void EventLoop::removeFdFrom(std::lock_guard<std::mutex> &lock,
                             std::vector<FdInfo> &fds,
                             const std::vector<InternalEvent> &removeFds) {
  if (!removeFds.empty()) {
    for (auto &fdInfo : removeFds) {
      auto foundFd = findFdIn(fdInfo.fd_, fds);
      if (foundFd != fds.end()) {
        auto itemToRemove = std::remove(foundFd->callbacks_.begin(), foundFd->callbacks_.end(), fdInfo.callback_);
        foundFd->callbacks_.erase(itemToRemove);
        if (foundFd->callbacks_.empty()) {
          fds.erase(foundFd);
        }
      }
    }
  }
}

void EventLoop::initFds(std::vector<FdInfo> &fds,
                        fd_set &fdSet,
                        int &maxFd) const {
  for (const auto &fdInfo : fds) {
    FD_SET(fdInfo.fd_, &fdSet);
    if (fdInfo.fd_ > maxFd) {
      maxFd = fdInfo.fd_;
    }
  }
}

void EventLoop::handleLoopEvents(fd_set fdSet) {
  if (FD_ISSET(event_loop_fd_, &fdSet)) {
    std::lock_guard<std::mutex> lock(internal_mtx_);
    eventfd_t updatedEvent;
    eventfd_read(event_loop_fd_, &updatedEvent);
    if (updatedEvent > 0) {
      addFdTo(lock, read_fds_, add_read_fds_);
      addFdTo(lock, write_fds_, add_write_fds_);
      addFdTo(lock, error_fds_, add_error_fds_);
      removeFdFrom(lock, read_fds_, remove_read_fds_);
      removeFdFrom(lock, write_fds_, remove_write_fds_);
      removeFdFrom(lock, error_fds_, remove_error_fds_);
      eventfd_write(event_loop_fd_, 0);
    }
  }
}

void EventLoop::handleFdsEvents(std::vector<FdInfo> &fds, fd_set &fdSet) {
  for (const auto &fdInfo : fds) {
    if (FD_ISSET(fdInfo.fd_, &fdSet)) {
      for (const auto &callback : fdInfo.callbacks_) {
        callback(fdInfo.fd_);
      }
    }
  }
}

std::vector<EventLoop::FdInfo>::iterator
EventLoop::findFdIn(const int fd, std::vector<FdInfo> &fds) {
  auto foundFd = std::find_if(fds.begin(), fds.end(),
                              [fd](const FdInfo &fdInfo) {
                                return fdInfo.fd_ == fd;
                              });
  return foundFd;
}

void EventLoop::registerFdEvents(const int fd,
                                 const FdEventType eventType,
                                 void(*callback)(int)) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  switch (eventType) {
    case READ: {
      add_read_fds_.emplace_back(fd, callback);
      break;
    }
    case WRITE: {
      add_write_fds_.emplace_back(fd, callback);
      break;
    }
    case ERROR: {
      add_error_fds_.emplace_back(fd, callback);
      break;
    }
  }
  eventfd_t updated = 1;
  eventfd_write(event_loop_fd_, updated);
}

void EventLoop::unregisterFdEvents(const int fd,
                                   const FdEventType eventType,
                                   void(*callback)(int)) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  switch (eventType) {
    case READ: {
      remove_read_fds_.emplace_back(fd, callback);
      break;
    }
    case WRITE: {
      remove_write_fds_.emplace_back(fd, callback);
      break;
    }
    case ERROR: {
      remove_error_fds_.emplace_back(fd, callback);
      break;
    }
  }
  eventfd_t updated = 1;
  eventfd_write(event_loop_fd_, updated);
}

}

}

}