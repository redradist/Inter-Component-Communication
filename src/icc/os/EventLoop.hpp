/**
 * @file EventLoop.hpp
 * @author Denis Kotov
 * @date 1 Jun 2019
 * @brief
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_OS_POSIX_EVENTLOOP_HPP
#define ICC_OS_POSIX_EVENTLOOP_HPP

#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include <icc/EventLoop.hpp>
#include <icc/_private/helpers/function_wrapper.hpp>

namespace icc {

namespace os {

class Timer;

class EventLoop : public IEventLoop {
 public:
  struct OSObject;
  enum ListenerEventType {
    READ,
    WRITE,
    ERROR
  };

  static EventLoop & getDefaultInstance();
  static std::shared_ptr<EventLoop> createEventLoop();
  ~EventLoop();

  void run() override;
  void stop() override;
  std::shared_ptr<IChannel> createChannel() override;
  std::thread::id getThreadId() const override;
  bool isRun() const override;

  std::shared_ptr<Timer> createTimer();

  void registerObjectEvents(const OSObject & osObject,
                            ListenerEventType eventType,
                            icc::_private::helpers::function_wrapper<void(int)> callback);
  void unregisterObjectEvents(const OSObject & osObject,
                              ListenerEventType eventType,
                              icc::_private::helpers::function_wrapper<void(int)> callback);

 private:
  struct InternalEvent;
  struct ListenerInfo;

  EventLoop();
  explicit EventLoop(std::nullptr_t);

  void addFdTo(std::lock_guard<std::mutex>& lock,
               std::vector<ListenerInfo>& listeners,
               const std::vector<InternalEvent>& addListeners);

  void removeFdFrom(std::lock_guard<std::mutex>& lock,
                    std::vector<ListenerInfo>& listeners,
                    const std::vector<InternalEvent>& removeListeners);

  void initListenObjects(std::vector<ListenerInfo> &fds, fd_set &fdSet, int &maxFd) const;

  void handleLoopEvents(fd_set fdSet);

  void handleListenersEvents(std::vector<ListenerInfo> &fds, fd_set &fdSet);

  static std::vector<EventLoop::ListenerInfo>::iterator
  findFdIn(const OSObject & osObject, std::vector<ListenerInfo>& fds);

  std::atomic<std::thread::id> event_loop_thread_id_;
  std::thread event_loop_thread_;
  std::atomic_bool execute_{true};
  OSObject * event_loop_object_ = nullptr;
  std::mutex internal_mtx_;
  std::vector<InternalEvent> add_read_listeners_;
  std::vector<InternalEvent> remove_read_listeners_;
  std::vector<InternalEvent> add_write_listeners_;
  std::vector<InternalEvent> remove_write_listeners_;
  std::vector<InternalEvent> add_error_listeners_;
  std::vector<InternalEvent> remove_error_listeners_;
  std::vector<ListenerInfo> read_listeners_;
  std::vector<ListenerInfo> write_listeners_;
  std::vector<ListenerInfo> error_listeners_;
};

}

}

#endif //ICC_OS_POSIX_EVENTLOOP_HPP
