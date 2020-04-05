//
// Created by redra on 18.07.19.
//

#ifndef FORECAST_SERVERSOCKET_HPP
#define FORECAST_SERVERSOCKET_HPP

#include <vector>
#include <memory>
#include "IServerSocket.hpp"
#include "ISocket.hpp"
#include "IServerSocketListener.hpp"

namespace icc {

namespace os {

class ServerSocket : public IServerSocket {
 public:
  static std::shared_ptr<IServerSocket> createServerSocket(std::string _address, uint16_t _port, uint16_t _numQueue);
  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(std::shared_ptr<IServerSocketListener> _listener) override;

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(IServerSocketListener * _listener) override;

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(std::shared_ptr<IServerSocketListener> _listener) override;

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(IServerSocketListener * _listener) override;
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
