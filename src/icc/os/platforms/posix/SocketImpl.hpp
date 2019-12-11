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

struct SendChunk {
  const unsigned id_;
  ChunkData data_;
};

struct ReceiveChunk {
  std::unique_ptr<std::promise<ChunkData>> promise_ptr_;
};

class Socket::SocketImpl : public ISocket {
 public:
  ~SocketImpl() = default;

  void send(ChunkData _data) override;
  std::future<void> sendAsync(ChunkData _data) override;
  SharedChunkData receive() override;
  std::future<SharedChunkData> receiveAsync() override;

 private:
  friend class EventLoop;

  explicit SocketImpl(const Handle & socketHandle);
  void onSocketDataAvailable(const Handle &_);
  void onSocketBufferAvailable(const Handle &_);
  void notifyAllSendListeners(unsigned _dataId);

  Handle socket_handle_{kInvalidHandle};
  std::unique_ptr<uint8_t[]> receive_buffer_ptr_;
  std::deque<SendChunk> send_chunks_queue_;
  std::unordered_map<unsigned, std::promise<void>> async_send_chunk_promises_;
  std::unique_ptr<std::promise<SharedChunkData>> async_receive_chunk_;
  std::atomic<size_t> current_sent_data_size_{0};
  std::atomic_bool data_available_event_{false};
  std::atomic_bool buffer_available_event_{false};
  unsigned next_send_chunk_id_ = 0;
  unsigned next_receive_chunk_id_ = 0;
  std::mutex write_mtx_;
  std::mutex read_mtx_;
  std::condition_variable cond_var_;
};

}

}

#endif //ICC_SOCKETIMPL_HPP
