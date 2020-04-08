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

std::shared_ptr<Socket>
ServerSocket::ServerSocketImpl::accept() {
  return acceptAsync().get();
}

std::future<std::shared_ptr<Socket>>
ServerSocket::ServerSocketImpl::acceptAsync() {
  auto promiseResult = std::promise<std::shared_ptr<Socket>>{};
  auto futureResult = promiseResult.get_future();

  std::lock_guard<std::mutex> lock{mtx_};
  accept_queue_.emplace_back(std::move(promiseResult));

  return futureResult;
}

void ServerSocket::ServerSocketImpl::onSocketDataAvailable(const Handle &_) {
  std::lock_guard<std::mutex> lock{mtx_};
  while (!accept_queue_.empty() && !is_blocking_) {
    const int kSock = ::accept(socket_handle_.fd_, nullptr, nullptr);
    if (kSock < 0) {
      perror("accept");
      break;
    }
    auto clientSocket = EventLoop::getDefaultInstance().createSocket(Handle{kSock});
    if (clientSocket) {
      client_sockets_.push_back(clientSocket);
      auto & acceptReq = accept_queue_.front();
      acceptReq.set_value(clientSocket);
      accept_queue_.pop_front();
    }
  }
}

}

}
