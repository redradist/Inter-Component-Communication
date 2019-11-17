//
// Created by redra on 18.07.19.
//

#ifndef FORECAST_SOCKET_HPP
#define FORECAST_SOCKET_HPP

#include "ISocketListener.hpp"
#include "ISocket.hpp"

namespace icc {

namespace os {

class Socket : public ISocket {
 public:
  static std::shared_ptr<Socket> createSocket();
  ~Socket() = default;

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
  class SocketImpl;

  explicit Socket(std::shared_ptr<SocketImpl> implPtr);

  std::shared_ptr<SocketImpl> impl_ptr_;
};

}

}

#endif //FORECAST_SOCKET_HPP
