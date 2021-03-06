//
// Created by redra on 18.07.19.
//

#include <system_error>
#include <iostream>
#include "SocketImpl.hpp"

namespace icc {

namespace os {

constexpr uint16_t RECEIVE_BUFFER_SIZE = 4096;

Socket::SocketImpl::SocketImpl(const Handle & socketHandle)
    : socket_handle_{socketHandle}
    , receive_buffer_ptr_{new uint8_t[RECEIVE_BUFFER_SIZE]{0}} {
}

void Socket::SocketImpl::send(ChunkData _data) {
  sendAsync(_data).get();
}

std::future<void>
Socket::SocketImpl::sendAsync(ChunkData _data) {
  auto promiseResult = std::promise<void>{};
  auto futureResult = promiseResult.get_future();

  std::lock_guard<std::mutex> lock{write_mtx_};
  send_chunks_queue_.emplace_back(SentChunkData{std::move(_data), 0}, std::move(promiseResult));
  send_chunks_available_event_.store(!send_chunks_queue_.empty(), std::memory_order_release);
  if (buffer_available_event_.load(std::memory_order_acquire)) {
    sendDataTo();
  }

  return futureResult;
}

ChunkData
Socket::SocketImpl::receive() {
  return receiveAsync().get();
}

std::future<ChunkData>
Socket::SocketImpl::receiveAsync() {
  auto promiseResult = std::promise<ChunkData>{};
  auto futureResult = promiseResult.get_future();

  std::lock_guard<std::mutex> lock{read_mtx_};
  read_requests_queue_.push_back(std::move(promiseResult));
  read_requests_available_event_.store(!read_requests_queue_.empty(), std::memory_order_release);
  if (data_available_event_.load(std::memory_order_acquire)) {
    readDataFrom();
  }

  return futureResult;
}

void Socket::SocketImpl::onSocketDataAvailable(const Handle &_) {
  data_available_event_.store(true, std::memory_order_release);
  std::lock_guard<std::mutex> lock{read_mtx_};
  while (!read_requests_queue_.empty()) {
    readDataFrom();
    if (is_blocking_) {
      break;
    }
  }
}

void Socket::SocketImpl::readDataFrom() {
  int recvError = NO_ERROR;
  auto & promiseChunk = read_requests_queue_.front();
  do {
    DWORD recvLen;
    DWORD flags = 0;
    WSABUF wsaBuffer;
    wsaBuffer.buf = reinterpret_cast<char *>(receive_buffer_ptr_.get());
    wsaBuffer.len = RECEIVE_BUFFER_SIZE;
    int wsaRecvError = ::WSARecv(reinterpret_cast<SOCKET>(socket_handle_.handle_),
                                 &wsaBuffer, 1, &recvLen, &flags, nullptr, nullptr);
    int lastRecvError = WSAGetLastError();
    if (wsaRecvError != SOCKET_ERROR && recvLen > 0) {
      chunk_.insert(chunk_.end(), receive_buffer_ptr_.get(), receive_buffer_ptr_.get() + recvLen);
    } else if (0 == recvLen || lastRecvError == WSAEWOULDBLOCK || lastRecvError == WSATRY_AGAIN) {
      data_available_event_.store(false, std::memory_order_release);
      break;
    } else {
      recvError = lastRecvError;
      break;
    }
  } while (!is_blocking_);
  if (recvError != NO_ERROR) {
    promiseChunk.set_exception(
        std::make_exception_ptr(
            std::system_error(recvError, std::system_category(), "Socket send error")
        )
    );
    chunk_.clear();
    read_requests_queue_.pop_front();
  } else if (!chunk_.empty()) {
    promiseChunk.set_value(std::move(chunk_));
    chunk_.clear();
    read_requests_queue_.pop_front();
  }
  read_requests_available_event_.store(!read_requests_queue_.empty(), std::memory_order_release);
}

void Socket::SocketImpl::onSocketBufferAvailable(const Handle &_) {
  buffer_available_event_.store(true, std::memory_order_release);
  std::lock_guard<std::mutex> lock{write_mtx_};
  while (!send_chunks_queue_.empty()) {
    sendDataTo();
    if (is_blocking_) {
      break;
    }
  }
}

void Socket::SocketImpl::sendDataTo() {
  auto & chunk = send_chunks_queue_.front();
  int sendError = NO_ERROR;
  do {
    const size_t kDataOffset = chunk.first.sent_data_size_;
    const size_t kDataSize = chunk.first.chunk_.size() - kDataOffset;

    DWORD sendLen;
    DWORD flags = 0;
    WSABUF wsaBuffer;
    wsaBuffer.buf = reinterpret_cast<char *>(chunk.first.chunk_.data() + kDataOffset);
    wsaBuffer.len = kDataSize;
    int wsaSendError = ::WSASend(reinterpret_cast<SOCKET>(socket_handle_.handle_),
                                 &wsaBuffer, 1, &sendLen, flags, nullptr, nullptr);
    int lastSendError = WSAGetLastError();
    if (wsaSendError != SOCKET_ERROR && sendLen > 0) {
      chunk.first.sent_data_size_ += sendLen;
    } else if (0 == sendLen || lastSendError == WSAEWOULDBLOCK || lastSendError == WSATRY_AGAIN) {
      buffer_available_event_.store(false, std::memory_order_release);
      break;
    } else {
      sendError = lastSendError;
      break;
    }
  } while (!is_blocking_ && chunk.first.sent_data_size_ < chunk.first.chunk_.size());
  if (sendError != NO_ERROR) {
    chunk.second.set_exception(
        std::make_exception_ptr(
            std::system_error(sendError, std::system_category(), "Socket send error")
        )
    );
    send_chunks_queue_.pop_front();
  } else if (chunk.first.sent_data_size_ == chunk.first.chunk_.size()) {
    chunk.second.set_value();
    send_chunks_queue_.pop_front();
  }
  send_chunks_available_event_.store(!send_chunks_queue_.empty(), std::memory_order_release);
}

}

}
