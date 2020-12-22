//
// Created by redra on 05.04.20.
//

#include <algorithm>
#include <icc/os/EventLoop.hpp>
#include "ServerSocketImpl.hpp"

namespace icc {

namespace os {

typedef struct _PER_HANDLE_DATA
{
  SOCKET        Socket;
  SOCKADDR_STORAGE  ClientAddr;
  // Other information useful to be associated with the handle
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

ServerSocket::ServerSocketImpl::ServerSocketImpl(const Handle & socketHandle, const Handle & ioCompletionPort)
    : socket_handle_{socketHandle}
    , io_completion_port_{ioCompletionPort} {
  thr_ = std::thread([=] {
    std::unique_lock<std::mutex> lock{mtx_};
    var_.wait(lock, [=] {
      return !accept_queue_.empty();
    });
    waitAcceptAsync();
  });
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
  var_.notify_one();

  return futureResult;
}

void ServerSocket::ServerSocketImpl::waitAcceptAsync() {
  while (!accept_queue_.empty() && !is_blocking_) {
    SOCKADDR_IN saRemote;
    SOCKET Accept;
    int RemoteLen;

    RemoteLen = sizeof(saRemote);
    Accept = ::WSAAccept(reinterpret_cast<SOCKET>(socket_handle_.handle_), nullptr, nullptr, nullptr, 0);

    auto clientSocket = EventLoop::getDefaultInstance().createSocket(Handle{reinterpret_cast<HANDLE>(Accept)});
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
