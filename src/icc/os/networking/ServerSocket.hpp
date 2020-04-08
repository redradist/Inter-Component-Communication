//
// Created by redra on 18.07.19.
//

#ifndef FORECAST_SERVERSOCKET_HPP
#define FORECAST_SERVERSOCKET_HPP

#include <vector>
#include <memory>
#include "IServerSocket.hpp"
#include "ISocket.hpp"
#include "Socket.hpp"

namespace icc {

namespace os {

class ServerSocket : public IServerSocket {
 public:
  static std::shared_ptr<ServerSocket> createServerSocket(std::string _address, uint16_t _port, uint16_t _numQueue);

  std::shared_ptr<Socket> accept();
  std::future<std::shared_ptr<Socket>> acceptAsync();
  /**
   * Method is used to return all client sockets
   * @return Client sockets
   */
  const std::vector<std::shared_ptr<Socket>>& getClientSockets() const override;

 private:
  friend class EventLoop;
  class ServerSocketImpl;

  explicit ServerSocket(std::shared_ptr<ServerSocketImpl> implPtr);

  std::shared_ptr<ServerSocketImpl> impl_ptr_;
};

}

}

#endif //FORECAST_SERVERSOCKET_HPP
