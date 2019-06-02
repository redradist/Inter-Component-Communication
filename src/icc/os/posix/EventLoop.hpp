/**
 * @file EventLoop.hpp
 * @author Denis Kotov
 * @date 1 Jun 2019
 * @brief
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_OS_EVENTLOOP_HPP
#define ICC_OS_EVENTLOOP_HPP

#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include <icc/os/OSEventLoop.hpp>

namespace icc {

namespace os {

namespace posix {

class EventLoop : public icc::os::EventLoop {
 public:
  class Timer;

  enum FdEventType {
    READ,
    WRITE,
    ERROR
  };

  ~EventLoop();

  static EventLoop & getDefaultInstance();
  static std::shared_ptr<EventLoop> createInstance();
  void registerFdEvents(int fd, FdEventType eventType, void(*callback)(int));
  void unregisterFdEvents(int fd, FdEventType eventType, void(*callback)(int));

  void run() override;
  void stop() override;
  std::shared_ptr<IChannel> createChannel() override;
  std::thread::id getThreadId() const override;
  bool isRun() const override;

 private:
  struct InternalEvent;
  struct FdInfo;

  EventLoop(std::nullptr_t);
  EventLoop() = default;

  void addFdTo(std::lock_guard<std::mutex> &lock,
               std::vector<FdInfo> &fds,
               const std::vector<InternalEvent> &addFds);

  void removeFdFrom(std::lock_guard<std::mutex> &lock,
                    std::vector<FdInfo> &fds,
                    const std::vector<InternalEvent> &removeFds);

  void initFds(std::vector<FdInfo> &fds, fd_set &fdSet, int &maxFd) const;

  void handleLoopEvents(fd_set fdSet);

  void handleFdsEvents(std::vector<FdInfo> &fds, fd_set &fdSet);

  static std::vector<EventLoop::FdInfo>::iterator
  findFdIn(const int fd, std::vector<FdInfo> &fds);

  std::thread event_loop_thread_;
  std::atomic_bool execute_{true};
  int event_loop_fd_;
  std::mutex internal_mtx_;
  std::vector<InternalEvent> add_read_fds_;
  std::vector<InternalEvent> remove_read_fds_;
  std::vector<InternalEvent> add_write_fds_;
  std::vector<InternalEvent> remove_write_fds_;
  std::vector<InternalEvent> add_error_fds_;
  std::vector<InternalEvent> remove_error_fds_;
  std::vector<FdInfo> read_fds_;
  std::vector<FdInfo> write_fds_;
  std::vector<FdInfo> error_fds_;
};

}

}

}

#endif //ICC_OS_EVENTLOOP_HPP
