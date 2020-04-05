//
// Created by redra on 18.07.19.
//

#ifndef ICC_SOCKETIMPL_HPP
#define ICC_SOCKETIMPL_HPP

#include <memory>
#include <chrono>
#include <vector>
#include <deque>
#include <future>
#include <unordered_map>

#include <icc/ITimerListener.hpp>
#include <icc/os/EventLoop.hpp>
#include <icc/os/networking/Socket.hpp>
#include <bits/unordered_map.h>

#include "Common.hpp"

namespace icc {

namespace os {

struct Handle;

class Socket::SocketImpl : public ISocket {
 public:
  ~SocketImpl() = default;

  void send(ChunkData _data) override;
  std::future<void> sendAsync(ChunkData _data) override;
  ChunkData receive() override;
  std::future<ChunkData> receiveAsync() override;

 private:
  friend class EventLoop;

  explicit SocketImpl(const Handle & socketHandle);
  void onSocketDataAvailable(const Handle &_);
  void onSocketBufferAvailable(const Handle &_);
  void setBlockingMode(bool isBlocking);

  Handle socket_handle_{kInvalidHandle};
  bool is_blocking_ = true;
  std::unique_ptr<uint8_t[]> receive_buffer_ptr_;
  std::deque<std::pair<ChunkData, std::promise<void>>> send_chunks_queue_;
  std::atomic_bool send_chunks_available_event_{true};
  std::deque<std::promise<ChunkData>> read_requests_queue_;
  std::atomic_bool read_requests_available_event_{true};
  std::mutex write_mtx_;
  std::mutex read_mtx_;
};

inline
void Socket::SocketImpl::setBlockingMode(bool isBlocking) {
  is_blocking_ = isBlocking;
}

}

}

#endif //ICC_SOCKETIMPL_HPP
