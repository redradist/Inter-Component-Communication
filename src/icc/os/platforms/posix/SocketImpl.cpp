//
// Created by redra on 18.07.19.
//

extern "C" {

#include <sys/socket.h>

}

#include "SocketImpl.hpp"

namespace icc {

namespace os {

constexpr uint16_t RECEIVE_BUFFER_SIZE = 4096;

Socket::SocketImpl::SocketImpl(const Handle & socketHandle)
    : socket_handle_{socketHandle}
    , receive_buffer_ptr_{new uint8_t[RECEIVE_BUFFER_SIZE]{0}} {
}

void Socket::SocketImpl::send(ChunkData _data) {
  std::unique_lock<std::mutex> lock{write_mtx_};
  const unsigned sendChunkId = next_send_chunk_id_++;
  send_chunks_queue_.push_back(SendChunk{sendChunkId, std::move(_data)});
  cond_var_.wait(lock, [this, sendChunkId] {
    return send_chunks_queue_.front().id_ == sendChunkId;
  });
  current_sent_data_size_.store(0, std::memory_order_release);
  SendChunk & chunk = send_chunks_queue_.front();
  while (current_sent_data_size_.load(std::memory_order_acquire) < chunk.data_.size()) {
    const size_t kDataOffset = current_sent_data_size_.load(std::memory_order_acquire);
    const size_t kDataSize = chunk.data_.size() - kDataOffset;
    buffer_available_event_.store(false, std::memory_order_release);
    const ssize_t size = ::send(socket_handle_.fd_,
                            chunk.data_.data() + kDataOffset,
                                kDataSize,
                           0);
    if (size != kDataSize) {
      if (errno != EWOULDBLOCK && errno != EAGAIN) {
        break;
      }
      current_sent_data_size_.fetch_add(size, std::memory_order_release);
      cond_var_.wait(lock, [this] {
        return buffer_available_event_.load(std::memory_order_acquire);
      });
    }
  }
}

std::future<void> Socket::SocketImpl::sendAsync(ChunkData _data) {
  std::promise<void> promiseResult{};
  std::future<void> futureResult = promiseResult.get_future();
  std::unique_lock<std::mutex> lock{write_mtx_};
  if (send_chunks_queue_.empty()) {
    current_sent_data_size_.store(0, std::memory_order_release);
    SendChunk & chunk = send_chunks_queue_.front();
    const size_t kDataSize = chunk.data_.size();
    buffer_available_event_.store(false, std::memory_order_release);
    const ssize_t size = ::send(socket_handle_.fd_,
                                chunk.data_.data(),
                                kDataSize,
                                0);
    if (size != kDataSize) {
      if (errno != EWOULDBLOCK && errno != EAGAIN) {
        promiseResult.set_exception(nullptr);
      } else {
        current_sent_data_size_.fetch_add(size, std::memory_order_release);
      }
    } else {
      promiseResult.set_value();
    }
  } else {
    const unsigned sendChunkId = next_send_chunk_id_++;
    send_chunks_queue_.push_back(SendChunk{sendChunkId, std::move(_data)});
    async_send_chunk_promises_[sendChunkId] = std::move(promiseResult);
  }
  return std::move(futureResult);
}

SharedChunkData Socket::SocketImpl::receive() {
  auto chunkPtr = std::make_shared<ChunkData>();
  std::unique_lock<std::mutex> lock{read_mtx_};
  ssize_t recvLen = ::recv(socket_handle_.fd_, receive_buffer_ptr_.get(), RECEIVE_BUFFER_SIZE, 0);
  if (recvLen == -1) {
    printf("errno %d\n", errno);
    chunkPtr = nullptr;
  } else {
    lock.unlock();
    chunkPtr->insert(chunkPtr->end(), receive_buffer_ptr_.get(), receive_buffer_ptr_.get()+recvLen);
  }
  return chunkPtr;
}

std::future<SharedChunkData> Socket::SocketImpl::receiveAsync() {
  auto promiseResult = std::unique_ptr<std::promise<SharedChunkData>>(new std::promise<SharedChunkData>{});
  auto futureResult = promiseResult->get_future();
  std::unique_lock<std::mutex> lock{read_mtx_};
  if (data_available_event_.load(std::memory_order_acquire)) {
    auto chunkData = std::make_shared<ChunkData>();
    ssize_t recvLen = ::recv(socket_handle_.fd_, receive_buffer_ptr_.get(), RECEIVE_BUFFER_SIZE, 0);
    lock.unlock();
    if (recvLen == -1) {
      printf("errno %d\n", errno);
    } else {
      chunkData->insert(chunkData->end(), receive_buffer_ptr_.get(), receive_buffer_ptr_.get()+recvLen);
      promiseResult->set_value(chunkData);
    }
  } else {
    if (async_receive_chunk_) {
      //NOTE(redra): Only one async read could be done at the same time
      promiseResult->set_exception(nullptr);
    } else {
      async_receive_chunk_ = std::move(promiseResult);
    }
  }
  return futureResult;
}

void Socket::SocketImpl::onSocketDataAvailable(const Handle &_) {
  auto promiseResult = std::unique_ptr<std::promise<SharedChunkData>>(new std::promise<SharedChunkData>{});
  auto futureResult = promiseResult->get_future();
  std::unique_lock<std::mutex> lock{read_mtx_};
  data_available_event_.store(true, std::memory_order_release);
  if (data_available_event_.load(std::memory_order_acquire)) {
    std::shared_ptr<ChunkData> chunkPtr = std::make_shared<ChunkData>();
    ssize_t recvLen = ::recv(socket_handle_.fd_, receive_buffer_ptr_.get(), RECEIVE_BUFFER_SIZE, 0);
    lock.unlock();
    if (recvLen == -1) {
      printf("errno %d\n", errno);
    } else {
      chunkPtr->insert(chunkPtr->end(), receive_buffer_ptr_.get(), receive_buffer_ptr_.get()+recvLen);
      promiseResult->set_value(chunkPtr);
    }
  } else {
    if (async_receive_chunk_) {
      //NOTE(redra): Only one async read could be done at the same time
      promiseResult->set_exception(nullptr);
    } else {
      async_receive_chunk_ = std::move(promiseResult);
    }
  }
}

void Socket::SocketImpl::onSocketBufferAvailable(const Handle &_) {
  if (!send_chunks_queue_.empty() && async_send_chunk_promises_.count(send_chunks_queue_.front().id_) > 0) {
    SendChunk & chunk = send_chunks_queue_.front();
    const size_t kDataOffset = current_sent_data_size_.load(std::memory_order_acquire);
    const size_t kDataSize = chunk.data_.size() - current_sent_data_size_.load(std::memory_order_acquire);
    buffer_available_event_.store(false, std::memory_order_release);
    ssize_t size = ::send(socket_handle_.fd_,
                          chunk.data_.data() + kDataOffset,
                          kDataSize,
                          0);
    if (size == kDataSize) {
      async_send_chunk_promises_[send_chunks_queue_.front().id_].set_value();
      async_send_chunk_promises_.erase(send_chunks_queue_.front().id_);
      notifyAllSendListeners(send_chunks_queue_.front().id_);
      send_chunks_queue_.pop_front();
    }
    current_sent_data_size_.fetch_add(size, std::memory_order_release);
  } else {
    std::unique_lock<std::mutex> lock{write_mtx_};
    buffer_available_event_.store(true, std::memory_order_release);
    cond_var_.notify_one();
  }
}

}

}
