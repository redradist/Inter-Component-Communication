//
// Created by redra on 08.06.19.
//

#ifndef POSIX_EVENTLOOPIMPL_HPP
#define POSIX_EVENTLOOPIMPL_HPP

#include <icc/os/EventLoop.hpp>

#include "OSObject.hpp"
#include "TimerImpl.hpp"

namespace icc {

namespace os {

class EventLoop::EventLoopImpl : public IEventLoop {
 public:
  EventLoopImpl() = default;
  EventLoopImpl(std::nullptr_t);
  ~EventLoopImpl();

  void run() override;
  void stop() override;
  std::shared_ptr<IChannel> createChannel() override;
  std::thread::id getThreadId() const override;
  bool isRun() const override;

  std::shared_ptr<Timer::TimerImpl> createTimerImpl();

  void registerObjectEvents(const OSObject & osObject,
                            const OSObjectEventType & eventType,
                            function_wrapper<void(const OSObject&)> callback);
  void unregisterObjectEvents(const OSObject & osObject,
                              const OSObjectEventType & eventType,
                              function_wrapper<void(const OSObject&)> callback);

 private:
  struct InternalEvent;
  struct OSObjectListeners;

  void addFdTo(std::lock_guard<std::mutex>& lock,
               std::vector<OSObjectListeners>& listeners,
               const std::vector<InternalEvent>& addListeners);
  void removeFdFrom(std::lock_guard<std::mutex>& lock,
                    std::vector<OSObjectListeners>& listeners,
                    const std::vector<InternalEvent>& removeListeners);
  void initFds(std::vector<OSObjectListeners> &fds, fd_set &fdSet, int &maxFd) const;
  void handleLoopEvents(fd_set fdSet);
  void handleOSObjectsEvents(std::vector<OSObjectListeners> &fds, fd_set &fdSet);
  static std::vector<OSObjectListeners>::iterator
  findOSObjectIn(const OSObject &osObject, std::vector<OSObjectListeners> &fds);

  std::atomic_bool execute_{true};
  std::thread event_loop_thread_;
  std::atomic<std::thread::id> event_loop_thread_id_;
  std::mutex internal_mtx_;
  OSObject event_loop_object_{-1};
  std::vector<InternalEvent> add_read_listeners_;
  std::vector<InternalEvent> remove_read_listeners_;
  std::vector<InternalEvent> add_write_listeners_;
  std::vector<InternalEvent> remove_write_listeners_;
  std::vector<InternalEvent> add_error_listeners_;
  std::vector<InternalEvent> remove_error_listeners_;
  std::vector<OSObjectListeners> read_listeners_;
  std::vector<OSObjectListeners> write_listeners_;
  std::vector<OSObjectListeners> error_listeners_;
};

struct EventLoop::EventLoopImpl::InternalEvent {
  OSObject object_;
  function_wrapper<void(const OSObject &)> callback_;

  InternalEvent(const OSObject fd, function_wrapper<void(const OSObject &)> callback)
      : object_{fd}, callback_{std::move(callback)} {
  }
};

struct EventLoop::EventLoopImpl::OSObjectListeners {
  OSObjectListeners(const OSObject fd)
      : object_{fd} {
  }

  OSObjectListeners(const OSObject fd, std::vector<function_wrapper<void(const OSObject &)>> callbacks)
      : object_{fd}, callbacks_{std::move(callbacks)} {
  }

  OSObject object_;
  std::vector<function_wrapper<void(const OSObject &)>> callbacks_;
};

}

}

#endif //POSIX_EVENTLOOPIMPL_HPP
