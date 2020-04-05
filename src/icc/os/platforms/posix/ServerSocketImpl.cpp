//
// Created by redra on 05.04.20.
//

extern "C" {

#include <sys/socket.h>

}

#include <algorithm>
#include <icc/os/EventLoop.hpp>
#include "ServerSocketImpl.hpp"

namespace icc {

namespace os {

ServerSocket::ServerSocketImpl::ServerSocketImpl(const Handle & socketHandle)
    : socket_handle_{socketHandle} {
}

void ServerSocket::ServerSocketImpl::addListener(std::shared_ptr<IServerSocketListener> _listener) {
  if (_listener) {
    std::lock_guard<std::mutex> lock(mtx_);
    listeners_.push_back(_listener);
  }
}

void ServerSocket::ServerSocketImpl::addListener(IServerSocketListener * _listener) {
  if (_listener) {
    std::lock_guard<std::mutex> lock(mtx_);
    listeners_ptr_.push_back(_listener);
  }
}

void ServerSocket::ServerSocketImpl::removeListener(std::shared_ptr<IServerSocketListener> _listener) {
  if (_listener) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto erased = std::remove_if(listeners_.begin(), listeners_.end(),
                                 [_listener](const std::weak_ptr<IServerSocketListener> & weakListener) {
                                   bool result = false;
                                   if (auto _observer = weakListener.lock()) {
                                     result = _observer == _listener;
                                   }
                                   return result;
                                 });
    listeners_.erase(erased);
  }
}

void ServerSocket::ServerSocketImpl::removeListener(IServerSocketListener * _listener) {
  if (_listener) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto erased = std::remove(listeners_ptr_.begin(), listeners_ptr_.end(), _listener);
    listeners_ptr_.erase(erased);
  }
}

void ServerSocket::ServerSocketImpl::onSocketDataAvailable(const Handle &_) {
  do {
    const int kSock = ::accept(socket_handle_.fd_, nullptr, nullptr);
    if (kSock < 0) {
      perror("accept");
      continue;
    }
    auto clientSocket = EventLoop::getDefaultInstance().createSocket(Handle{kSock});
    if (clientSocket) {
      client_sockets_.push_back(clientSocket);
      std::unique_lock<std::mutex> lock{mtx_};
      for (auto listener : listeners_ptr_) {
        listener->onNewClientSocket(clientSocket);
      }
      for (auto weakListener : listeners_) {
        if (auto listener = weakListener.lock()) {
          listener->onNewClientSocket(clientSocket);
        }
      }
      for (auto weakListenerIter = listeners_.begin(); weakListenerIter != listeners_.end();) {
        if (weakListenerIter->expired()) {
          weakListenerIter = listeners_.erase(weakListenerIter);
        } else {
          ++weakListenerIter;
        }
      }
    }
  } while (!is_blocking_);
}

}

}
