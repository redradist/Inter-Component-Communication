//
// Created by redra on 18.07.19.
//

extern "C" {

#include "sys/socket.h"

}

#include "SocketImpl.hpp"

namespace icc {

namespace os {

Socket::SocketImpl::SocketImpl(const Handle & socketHandle)
    : socket_handle_{socketHandle} {
}

void Socket::SocketImpl::send(ChunkData _data) {
  std::unique_lock<std::mutex> lock{mtx_};
  buffer_chunks_.push_back(SendChunk{std::move(_data), nullptr});
  cond_var_.wait(lock, [this] {
    return buffer_chunks_.front().promise_ptr_ == nullptr;
  });
  lock.unlock();
  current_sent_data_.store(0, std::memory_order_release);
  SendChunk & chunk = buffer_chunks_.front();
  while (current_sent_data_.load(std::memory_order_acquire) < chunk.data_.size()) {
    const size_t kDataOffset = current_sent_data_.load(std::memory_order_acquire);
    const size_t kDataSize = chunk.data_.size() - current_sent_data_.load(std::memory_order_acquire);
    buffer_available_event_.store(false, std::memory_order_release);
    ssize_t size;
    if ((size = ::send(socket_handle_.fd_,
                       chunk.data_.data() + kDataOffset,
                       kDataSize,
                       0)) != kDataSize) {
      current_sent_data_.fetch_add(size, std::memory_order_release);
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        printf("%s\n", errno == EAGAIN ? "EAGAIN" : "EWOULDBLOCK");
        lock.lock();
        cond_var_.wait(lock, [this] {
          return buffer_available_event_.load(std::memory_order_acquire);
        });
        lock.unlock();
      } else {
        printf("errno %d\n", errno);
        break;
      }
    }
  }
}

std::future<void> Socket::SocketImpl::sendAsync(ChunkData _data,
                                                ISocketSender &_sender) {
  std::unique_lock<std::mutex> lock{mtx_};
  auto promiseResult = std::unique_ptr<std::promise<void>>{new std::promise<void>{}};
  std::future<void> futureResult = promiseResult->get_future();
  buffer_chunks_.push_back(SendChunk{std::move(_data), std::move(promiseResult)});
  if (buffer_chunks_.size() == 1) {
    lock.unlock();
    current_sent_data_.store(0, std::memory_order_release);
    SendChunk & chunk = buffer_chunks_.front();
    const size_t kDataSize = chunk.data_.size();
    buffer_available_event_.store(false, std::memory_order_release);
    ssize_t size;
    if ((size = ::send(socket_handle_.fd_,
                       chunk.data_.data(),
                       kDataSize,
                       0)) != kDataSize) {
      current_sent_data_.fetch_add(size, std::memory_order_release);
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        printf("%s\n", errno == EAGAIN ? "EAGAIN" : "EWOULDBLOCK");
      } else {
        printf("errno %d\n", errno);
      }
    }
  }
  return std::move(futureResult);
}

ChunkData Socket::SocketImpl::receive() {
  return {};
}

std::future<ChunkData> Socket::SocketImpl::receiveAsync(ISocketReceiver & _receiver) {
  return std::future<ChunkData>{};
}

void Socket::SocketImpl::onSocketDataAvailable(const Handle &_) {

}

void Socket::SocketImpl::onSocketBufferAvailable(const Handle &_) {
  if (!buffer_chunks_.empty() && buffer_chunks_.front().promise_ptr_ != nullptr) {
    SendChunk & chunk = buffer_chunks_.front();
    const size_t kDataOffset = current_sent_data_.load(std::memory_order_acquire);
    const size_t kDataSize = chunk.data_.size() - current_sent_data_.load(std::memory_order_acquire);
    buffer_available_event_.store(false, std::memory_order_release);
    ssize_t size;
    if ((size = ::send(socket_handle_.fd_,
                       chunk.data_.data() + kDataOffset,
                       kDataSize,
                       0)) != kDataSize) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        printf("%s\n", errno == EAGAIN ? "EAGAIN" : "EWOULDBLOCK");
      } else {
        printf("errno %d\n", errno);
      }
    } else {
      chunk.promise_ptr_->set_value();
    }
    current_sent_data_.fetch_add(size, std::memory_order_release);
  } else {
    buffer_available_event_.store(true, std::memory_order_release);
    cond_var_.notify_one();
  }
}

void Socket::SocketImpl::addListener(ISocketListener *_listener) {

}

void addListener(std::shared_ptr<ISocketListener> _listener) {

}

void addListener(ISocketListener * _listener) {

}

void removeListener(std::shared_ptr<ISocketListener> _listener) {

}

void removeListener(ISocketListener * _listener) {

}

}

}
