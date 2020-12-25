//
// Created by redra on 08.06.19.
//

#ifndef POSIX_EVENTLOOPIMPL_HPP
#define POSIX_EVENTLOOPIMPL_HPP

#include <icc/os/EventLoop.hpp>

#include "Common.hpp"
#include "TimerImpl.hpp"
#include "SocketImpl.hpp"
#include "ServerSocketImpl.hpp"

namespace icc {

namespace os {

struct SocketsWorkerThreadParams {
  Handle io_completion_port_;
  std::atomic_bool & execute_;
};

class EventLoop::EventLoopImpl : public IEventLoop {
 public:
  EventLoopImpl() = default;
  explicit EventLoopImpl(std::nullptr_t);
  ~EventLoopImpl();

  void run() override;
  void stop() override;
  bool isRun() const override;

  std::shared_ptr<Timer::TimerImpl> createTimerImpl();
  std::shared_ptr<ServerSocket::ServerSocketImpl> createServerSocketImpl(std::string _address, uint16_t _port, uint16_t _numQueue);
  std::shared_ptr<ServerSocket::ServerSocketImpl> createServerSocketImpl(const Handle & _socketHandle);
  std::shared_ptr<Socket::SocketImpl> createSocketImpl(const std::string& _address, uint16_t _port);
  std::shared_ptr<Socket::SocketImpl> createSocketImpl(const Handle & _socketHandle);

  void registerObjectEvents(const Handle &osObject,
                            const long event,
                            function_wrapper<void(const Handle &)> callback);
  void unregisterObjectEvents(const Handle &osObject,
                              const long event,
                              function_wrapper<void(const Handle &)> callback);

 private:
  struct InternalEvent;
  struct HandleListeners;

  bool setSocketBlockingMode(SOCKET _fd, bool _isBlocking);
  void addFdTo(std::lock_guard<std::mutex> &lock,
               std::vector<HandleListeners> &listeners,
               const std::vector<InternalEvent> &addListeners);
  void removeFdFrom(std::lock_guard<std::mutex> &lock,
                    std::vector<HandleListeners> &listeners,
                    const std::vector<InternalEvent> &removeListeners);
  void initFds(std::vector<HandleListeners> &fds,
               WSAEVENT *eventArray,
               size_t numEvents) const;
  void handleLoopEvents(WSAEVENT eventObj);
  void handleHandlesEvents(std::vector<HandleListeners> &fds, WSAEVENT event);
  static std::vector<HandleListeners>::iterator
  findOSObjectIn(const Handle &osObject, std::vector<HandleListeners> &fds);

  Handle io_completion_port_;
  std::atomic_bool execute_{true};
  WSADATA wsa_data_;
  std::vector<HANDLE> sockets_worker_threads_;
  std::thread event_loop_thread_;
  std::unique_ptr<SocketsWorkerThreadParams> sockets_worker_thread_params_;
  std::mutex internal_mtx_;
  Handle event_loop_handle_{kInvalidHandle};
  std::vector<InternalEvent> add_event_listeners_;
  std::vector<InternalEvent> remove_event_listeners_;
  std::vector<HandleListeners> event_listeners_;
};

struct EventLoop::EventLoopImpl::InternalEvent {
  Handle object_;
  long event_;
  function_wrapper<void(const Handle &)> callback_;

  InternalEvent(const Handle fd, const long event, function_wrapper<void(const Handle &)> callback)
      : object_{fd}
      , event_{event}
      , callback_{std::move(callback)} {
  }
};

struct EventLoop::EventLoopImpl::HandleListeners {
  HandleListeners(const Handle fd, const WSAEVENT event)
      : handle_{fd}
      , event_{event} {
  }

  HandleListeners(const Handle fd,
                  const WSAEVENT event,
                  std::vector<function_wrapper<void(const Handle &)>> callbacks)
      : handle_{fd}
      , event_{event}
      , callbacks_{std::move(callbacks)} {
  }

  Handle handle_;
  WSAEVENT event_;
  std::vector<function_wrapper<void(const Handle &)>> callbacks_;
};

}

}

#endif //POSIX_EVENTLOOPIMPL_HPP
