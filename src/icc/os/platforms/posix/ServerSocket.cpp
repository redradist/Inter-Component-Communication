//
// Created by redra on 05.04.20.
//

#include <icc/os/EventLoop.hpp>
#include <icc/os/networking/ServerSocket.hpp>

#include "ServerSocketImpl.hpp"

namespace icc {

namespace os {

ServerSocket::ServerSocket(std::shared_ptr<ServerSocketImpl> implPtr)
    : impl_ptr_{std::move(implPtr)} {
}

std::shared_ptr<IServerSocket> ServerSocket::createServerSocket(const std::string _address, const uint16_t _port, const uint16_t _numQueue) {
  return EventLoop::getDefaultInstance().createServerSocket(_address, _port, _numQueue);
}

void ServerSocket::addListener(std::shared_ptr<IServerSocketListener> _listener) {
  if (_listener) {
    impl_ptr_->addListener(_listener);
  }
}

void ServerSocket::addListener(IServerSocketListener * _listener) {
  if (_listener) {
    impl_ptr_->addListener(_listener);
  }
}

void ServerSocket::removeListener(std::shared_ptr<IServerSocketListener> _listener) {
  if (_listener) {
    impl_ptr_->removeListener(_listener);
  }
}

void ServerSocket::removeListener(IServerSocketListener * _listener) {
  if (_listener) {
    impl_ptr_->removeListener(_listener);
  }
}

const std::vector<std::shared_ptr<Socket>>&
ServerSocket::getClientSockets() const {
  return impl_ptr_->getClientSockets();
}

}

}
