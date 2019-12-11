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
  static std::shared_ptr<Socket> createSocket(std::string _address, uint16_t _port);
  ~Socket() = default;

  void send(std::vector<uint8_t> _data) override;
  std::future<void> sendAsync(std::vector<uint8_t> _data) override;
  SharedChunkData receive() override;
  std::future<SharedChunkData> receiveAsync() override;

 private:
  friend class EventLoop;
  class SocketImpl;

  explicit Socket(std::shared_ptr<SocketImpl> implPtr);

  std::shared_ptr<SocketImpl> impl_ptr_;
};

}

}

#endif //FORECAST_SOCKET_HPP
