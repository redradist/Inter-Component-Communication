//
// Created by redra on 18.07.19.
//

#ifndef ICC_SOCKETIMPL_HPP
#define ICC_SOCKETIMPL_HPP

#include <memory>
#include <chrono>

#include <icc/ITimerListener.hpp>
#include <icc/os/EventLoop.hpp>
#include <icc/os/networking/Socket.hpp>

#include "os_objects.hpp"

namespace icc {

namespace os {

struct Handle;

class Socket::SocketImpl : public ISocket {
 public:
  ~SocketImpl() = default;

  void send(std::vector<uint8_t> _data) override;
  void sendAsync(std::vector<uint8_t> _data, ISocketSender &_sender) override;
  void receive() override;
  void receiveAsync(ISocketReceiver & _receiver) override;

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(std::shared_ptr<ISocketListener> _listener) override;

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(ISocketListener * _listener) override;

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(std::shared_ptr<ISocketListener> _listener) override;

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(ISocketListener * _listener) override;

 private:
  friend class EventLoop;

  explicit SocketImpl(const Handle & timerObject);

  Handle socket_object_{-1};
};

}

}

#endif //ICC_SOCKETIMPL_HPP
