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
#include <icc/Context.hpp>
#include <icc/_private/helpers/function_wrapper.hpp>
#include <icc/_private/api.hpp>

#include "IEventLoop.hpp"

namespace icc {

namespace os {

template <typename T>
using function_wrapper = icc::_private::helpers::function_wrapper<T>;

struct Handle;

class Timer;
class ServerSocket;
class Socket;

class ICC_PUBLIC EventLoop : public IEventLoop {
 public:
  static EventLoop & getDefaultInstance();
  static std::shared_ptr<EventLoop> createEventLoop();
  ~EventLoop();

  void run() override;
  void stop() override;
  bool isRun() const override;

  std::shared_ptr<Timer> createTimer();
  std::shared_ptr<ServerSocket> createServerSocket(std::string _address, uint16_t _port, uint16_t _numQueue);
  std::shared_ptr<ServerSocket> createServerSocket(const Handle & _serverSocketHandle);
  std::shared_ptr<Socket> createSocket(const std::string& _address, uint16_t _port);
  std::shared_ptr<Socket> createSocket(const Handle & _socketHandle);

  void registerObjectEvents(const Handle & osObject,
                            const long event,
                            function_wrapper<void(const Handle&)> callback);
  void unregisterObjectEvents(const Handle & osObject,
                              const long event,
                              function_wrapper<void(const Handle&)> callback);

 private:
  class EventLoopImpl;

  EventLoop();
  explicit EventLoop(std::nullptr_t);
  std::unique_ptr<EventLoopImpl> impl_ptr_;
};

}

}

#endif //ICC_OS_POSIX_EVENTLOOP_HPP
