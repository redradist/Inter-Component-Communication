//
// Created by redra on 05.04.20.
//

#ifndef ICC_SERVERSOCKETIMPL_HPP
#define ICC_SERVERSOCKETIMPL_HPP

#include <deque>
#include <atomic>
#include <icc/os/networking/ServerSocket.hpp>

#include "Common.hpp"

namespace icc {

namespace os {

struct Handle;

class ServerSocket::ServerSocketImpl {
 public:
  ~ServerSocketImpl() = default;

  std::shared_ptr<Socket> accept();
  std::future<std::shared_ptr<Socket>> acceptAsync();

  const std::vector<std::shared_ptr<Socket>>&
  getClientSockets() const;

 private:
  friend class EventLoop;

  explicit ServerSocketImpl(const Handle & socketHandle, const Handle & ioCompletionPort);
  void waitAcceptAsync();
  void setBlockingMode(bool isBlocking);

  std::thread thr_;
  Handle socket_handle_{kInvalidHandle};
  Handle io_completion_port_{kInvalidHandle};
  bool is_blocking_ = false;
  std::vector<std::shared_ptr<Socket>> client_sockets_;
  std::deque<std::promise<std::shared_ptr<Socket>>> accept_queue_;
  std::atomic_bool is_new_client_available_event_{false};
  std::condition_variable var_;

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
