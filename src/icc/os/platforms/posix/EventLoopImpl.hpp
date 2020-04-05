//
// Created by redra on 08.06.19.
//

#ifndef POSIX_EVENTLOOPIMPL_HPP
#define POSIX_EVENTLOOPIMPL_HPP

#include <icc/os/EventLoop.hpp>

#include "Common.hpp"
#include "TimerImpl.hpp"
#include "SocketImpl.hpp"

namespace icc {

namespace os {

class EventLoop::EventLoopImpl : public IContext {
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
  std::shared_ptr<Socket::SocketImpl> createSocketImpl(std::string _address, uint16_t _port);

  void registerObjectEvents(const Handle & osObject,
                            const EventType & eventType,
                            function_wrapper<void(const Handle&)> callback);
  void unregisterObjectEvents(const Handle & osObject,
                              const EventType & eventType,
                              function_wrapper<void(const Handle&)> callback);

 private:
  struct InternalEvent;
  struct HandleListeners;

  bool setSocketBlockingMode(int _fd, bool _isBlocking);
  void addFdTo(std::lock_guard<std::mutex>& lock,
               std::vector<HandleListeners>& listeners,
               const std::vector<InternalEvent>& addListeners);
  void removeFdFrom(std::lock_guard<std::mutex>& lock,
                    std::vector<HandleListeners>& listeners,
                    const std::vector<InternalEvent>& removeListeners);
  void initFds(std::vector<HandleListeners> &fds, fd_set &fdSet, int &maxFd) const;
  void handleLoopEvents(fd_set fdSet);
  void handleHandlesEvents(std::vector<HandleListeners> &fds, fd_set &fdSet);
  static std::vector<HandleListeners>::iterator
  findOSObjectIn(const Handle &osObject, std::vector<HandleListeners> &fds);

  std::atomic_bool execute_{true};
  std::thread event_loop_thread_;
  std::atomic<std::thread::id> event_loop_thread_id_;
  std::mutex internal_mtx_;
  Handle event_loop_handle_{kInvalidHandle};
  std::vector<InternalEvent> add_read_listeners_;
  std::vector<InternalEvent> remove_read_listeners_;
  std::vector<InternalEvent> add_write_listeners_;
  std::vector<InternalEvent> remove_write_listeners_;
  std::vector<InternalEvent> add_error_listeners_;
  std::vector<InternalEvent> remove_error_listeners_;
  std::vector<HandleListeners> read_listeners_;
  std::vector<HandleListeners> write_listeners_;
  std::vector<HandleListeners> error_listeners_;
};

struct EventLoop::EventLoopImpl::InternalEvent {
  Handle object_;
  function_wrapper<void(const Handle &)> callback_;

  InternalEvent(const Handle fd, function_wrapper<void(const Handle &)> callback)
      : object_{fd}, callback_{std::move(callback)} {
  }
};

struct EventLoop::EventLoopImpl::HandleListeners {
  HandleListeners(const Handle fd)
      : handle_{fd} {
  }

  HandleListeners(const Handle fd, std::vector<function_wrapper<void(const Handle &)>> callbacks)
      : handle_{fd}, callbacks_{std::move(callbacks)} {
  }

  Handle handle_;
  std::vector<function_wrapper<void(const Handle &)>> callbacks_;
};

}

}

#endif //POSIX_EVENTLOOPIMPL_HPP
