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

struct OSObject;
enum class OSObjectEventType;

template <typename T>
using function_wrapper = icc::_private::helpers::function_wrapper<T>;

class Timer;

class EventLoop : public IEventLoop {
 public:
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
                            const OSObjectEventType & eventType,
                            function_wrapper<void(const OSObject&)> callback);
  void unregisterObjectEvents(const OSObject & osObject,
                              const OSObjectEventType & eventType,
                              function_wrapper<void(const OSObject&)> callback);

 private:
  EventLoop();
  explicit EventLoop(std::nullptr_t);

  // NOTE(redra): Forward declaration to use pimpl
  class EventLoopImpl;
  std::unique_ptr<EventLoopImpl> impl_ptr_;
};

}

}

#endif //ICC_OS_POSIX_EVENTLOOP_HPP
