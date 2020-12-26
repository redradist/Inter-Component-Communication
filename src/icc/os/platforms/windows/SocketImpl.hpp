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

#include <icc/ITimerListener.hpp>
#include <icc/os/EventLoop.hpp>
#include <icc/os/networking/Socket.hpp>

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
  bool hasSendBufferSpace() const;
  bool hasRecvDataChunk() const;

 private:
  friend class EventLoop;

  explicit SocketImpl(const Handle & socketHandle);
  void readDataFrom();
  void sendDataTo();
  void onSocketDataAvailable(const Handle &_);
  void onSocketBufferAvailable(const Handle &_);
  void setBlockingMode(bool isBlocking);

  Handle socket_handle_{kInvalidHandle};
  bool is_blocking_ = true;
  ChunkData chunk_;
  std::unique_ptr<uint8_t[]> receive_buffer_ptr_;
  std::deque<std::pair<SentChunkData, std::promise<void>>> send_chunks_queue_;
  std::atomic_bool send_chunks_available_event_{false};
  std::atomic_bool buffer_available_event_{false};
  std::deque<std::promise<ChunkData>> read_requests_queue_;
  std::atomic_bool read_requests_available_event_{false};
  std::atomic_bool data_available_event_{false};
  std::mutex write_mtx_;
  std::mutex read_mtx_;
};

inline
bool Socket::SocketImpl::hasSendBufferSpace() const {
  return buffer_available_event_.load(std::memory_order_acquire);
}

inline
bool Socket::SocketImpl::hasRecvDataChunk() const {
  return data_available_event_.load(std::memory_order_acquire);
}

inline
void Socket::SocketImpl::setBlockingMode(bool isBlocking) {
  is_blocking_ = isBlocking;
}

}

}

#endif //ICC_SOCKETIMPL_HPP
