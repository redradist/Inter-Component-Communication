//
// Created by redra on 05.04.20.
//

#ifndef ICC_SERVERSOCKETIMPL_HPP
#define ICC_SERVERSOCKETIMPL_HPP

#include <atomic>
#include <icc/os/networking/ServerSocket.hpp>
#include <icc/os/networking/IServerSocketListener.hpp>

#include "Common.hpp"

namespace icc {

namespace os {

struct Handle;

class ServerSocket::ServerSocketImpl {
 public:
  ~ServerSocketImpl() = default;

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(std::shared_ptr<IServerSocketListener> _listener);

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(IServerSocketListener * _listener);

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(std::shared_ptr<IServerSocketListener> _listener);

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(IServerSocketListener * _listener);

  const std::vector<std::shared_ptr<Socket>>&
  getClientSockets() const;

 private:
  friend class EventLoop;

  explicit ServerSocketImpl(const Handle & socketHandle);
  void onSocketDataAvailable(const Handle &_);
  void setBlockingMode(bool isBlocking);

  Handle socket_handle_{kInvalidHandle};
  bool is_blocking_ = false;
  std::vector<std::shared_ptr<Socket>> client_sockets_;
  std::vector<IServerSocketListener*> listeners_ptr_;
  std::vector<std::weak_ptr<IServerSocketListener>> listeners_;
  std::mutex mtx_;
};

inline
void ServerSocket::ServerSocketImpl::setBlockingMode(bool isBlocking) {
  is_blocking_ = isBlocking;
}

inline
const std::vector<std::shared_ptr<Socket>>&
ServerSocket::ServerSocketImpl::getClientSockets() const {
  return client_sockets_;
}

}

}

#endif //ICC_SERVERSOCKETIMPL_HPP
