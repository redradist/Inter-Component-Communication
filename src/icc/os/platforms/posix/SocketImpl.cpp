//
// Created by redra on 18.07.19.
//

extern "C" {

#include <sys/socket.h>

}

#include <system_error>
#include "SocketImpl.hpp"

namespace icc {

namespace os {

constexpr uint16_t RECEIVE_BUFFER_SIZE = 4096;

Socket::SocketImpl::SocketImpl(const Handle & socketHandle)
    : socket_handle_{socketHandle}
    , receive_buffer_ptr_{new uint8_t[RECEIVE_BUFFER_SIZE]{0}} {
}

void Socket::SocketImpl::send(ChunkData _data) {
  sendAsync(_data).wait();
}

std::future<void>
Socket::SocketImpl::sendAsync(ChunkData _data) {
  auto promiseResult = std::promise<void>{};
  auto futureResult = promiseResult.get_future();

  std::unique_lock<std::mutex> lock{write_mtx_};
  send_chunks_queue_.emplace_back(std::move(_data), std::move(promiseResult));
  send_chunks_available_event_.store(!send_chunks_queue_.empty(), std::memory_order_release);

  return std::move(futureResult);
}

ChunkData
Socket::SocketImpl::receive() {
  return receiveAsync().get();
}

std::future<ChunkData>
Socket::SocketImpl::receiveAsync() {
  auto promiseResult = std::promise<ChunkData>{};
  auto futureResult = promiseResult.get_future();

  std::unique_lock<std::mutex> lock{read_mtx_};
  read_requests_queue_.push_back(std::move(promiseResult));
  read_requests_available_event_.store(!read_requests_queue_.empty(), std::memory_order_release);

  return futureResult;
}

void Socket::SocketImpl::onSocketDataAvailable(const Handle &_) {
  static ChunkData chunk;

  while (read_requests_available_event_.load(std::memory_order_acquire)) {
    std::unique_lock<std::mutex> lock{read_mtx_};
    auto & promiseChunk = read_requests_queue_.front();
    bool isRecvError = false;
    do {
      const ssize_t kRecvLen = ::recv(socket_handle_.fd_, receive_buffer_ptr_.get(), RECEIVE_BUFFER_SIZE, 0);
      if (kRecvLen > 0) {
        chunk.insert(chunk.end(), receive_buffer_ptr_.get(), receive_buffer_ptr_.get()+kRecvLen);
      } else if (0 == kRecvLen || errno == EWOULDBLOCK || errno == EAGAIN) {
        break;
      } else {
        isRecvError = true;
        break;
      }
    } while (!is_blocking_);
    if (isRecvError) {
      promiseChunk.set_exception(
          std::make_exception_ptr(
              std::system_error(errno, std::system_category(), "Socket send error")
          )
      );
      chunk.clear();
      read_requests_queue_.pop_front();
    } else if (!chunk.empty()) {
      promiseChunk.set_value(std::move(chunk));
      chunk.clear();
      read_requests_queue_.pop_front();
    }
    read_requests_available_event_.store(!read_requests_queue_.empty(), std::memory_order_release);
    if (is_blocking_) {
      break;
    }
  }
}

void Socket::SocketImpl::onSocketBufferAvailable(const Handle &_) {
  static size_t currentSentChunkDataSize = 0;

  while (send_chunks_available_event_.load(std::memory_order_acquire)) {
    std::unique_lock<std::mutex> lock{write_mtx_};
    auto & chunk = send_chunks_queue_.front();
    bool isSendError = false;
    do {
      const size_t kDataOffset = currentSentChunkDataSize;
      const size_t kDataSize = chunk.first.size() - kDataOffset;
      const ssize_t kSentLen = ::send(socket_handle_.fd_,
                                  chunk.first.data() + kDataOffset,
                                  kDataSize,
                                  0);

      if (kSentLen > 0) {
        currentSentChunkDataSize += kSentLen;
      } else if (0 == kSentLen || errno == EWOULDBLOCK || errno == EAGAIN) {
        break;
      } else {
        isSendError = true;
        break;
      }
    } while (!is_blocking_ && currentSentChunkDataSize < chunk.first.size());
    if (isSendError) {
      currentSentChunkDataSize = 0;
      chunk.second.set_exception(
          std::make_exception_ptr(
              std::system_error(errno, std::system_category(), "Socket send error")
          )
      );
      send_chunks_queue_.pop_front();
    } else if (currentSentChunkDataSize == chunk.first.size()) {
      currentSentChunkDataSize = 0;
      chunk.second.set_value();
      send_chunks_queue_.pop_front();
    }
    send_chunks_available_event_.store(!send_chunks_queue_.empty(), std::memory_order_release);
    if (is_blocking_) {
      break;
    }
  }
}

}

}
