/**
 * @file EventLoop.cpp
 * @author Denis Kotov
 * @date 1 Jun 2019
 * @brief
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

extern "C" {

#include <winsock2.h>
#include <windows.h>

}

#include <cstdlib>
#include <cerrno>

#include <iostream>
#include <algorithm>
#include <memory>

#include "EventLoopImpl.hpp"

namespace icc {

namespace os {

#define PORT 5150
#define DATA_BUFSIZE 8192

// Typedef definition
typedef struct {
  OVERLAPPED overlapped;
  WSABUF dataBuf;
  CHAR buffer[DATA_BUFSIZE];
  DWORD bytesSEND;
  DWORD bytesRECV;
} PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;

typedef struct _PER_HANDLE_DATA {
  SOCKET            socket;
  SOCKADDR_STORAGE  clientAddr;
  // Other information useful to be associated with the handle
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

EventLoop::EventLoopImpl::EventLoopImpl(std::nullptr_t)
    : EventLoopImpl() {
  event_loop_thread_ = std::thread(&EventLoop::EventLoopImpl::run, this);
}

EventLoop::EventLoopImpl::~EventLoopImpl() {
  stop();
  if (event_loop_thread_.joinable()) {
    event_loop_thread_.join();
  }
}

std::shared_ptr<Timer::TimerImpl> EventLoop::EventLoopImpl::createTimerImpl() {
  //TODO(redradist): Timer should be created (kTimerFd)
  const int kTimerFd = 0;
  auto timer = new Timer::TimerImpl(Handle{kTimerFd});
  function_wrapper<void(const Handle &)> callback(&Timer::TimerImpl::onTimerExpired, timer);
  registerObjectEvents(Handle{kTimerFd}, EventType::kRead, callback);
  return std::shared_ptr<Timer::TimerImpl>(timer,
                                           [this, callback](Timer::TimerImpl *timer) {
                                             unregisterObjectEvents(timer->timer_handle_, EventType::kRead, callback);
                                             //TODO(redradist): Timer should be closed
                                           });
}

bool
EventLoop::EventLoopImpl::setSocketBlockingMode(SOCKET _fd, bool _isBlocking) {
  if (_fd < 0) return false;

  unsigned long mode = _isBlocking ? 0 : 1;
  return (::ioctlsocket(_fd, FIONBIO, &mode) == 0) ? true : false;
}

//std::shared_ptr<ServerSocket::ServerSocketImpl> EventLoop::EventLoopImpl::createServerSocketImpl(std::string _address, uint16_t _port, uint16_t _numQueue) {
//  const int kServerSocketFd = ::socket(AF_INET, SOCK_STREAM, 0);
//  if(kServerSocketFd < 0) {
//    perror("socket");
//    return nullptr;
//  }
//
//  auto socketRawPtr = new ServerSocket::ServerSocketImpl(Handle{kServerSocketFd});
//  function_wrapper<void(const Handle&)> readCallback(&ServerSocket::ServerSocketImpl::onSocketDataAvailable, socketRawPtr);
//  registerObjectEvents(Handle{kServerSocketFd}, EventType::READ, readCallback);
//  auto socketPtr = std::shared_ptr<ServerSocket::ServerSocketImpl>(socketRawPtr,
//  [this, readCallback](ServerSocket::ServerSocketImpl* serverSocket) {
//    unregisterObjectEvents(serverSocket->socket_handle_, EventType::READ, readCallback);
//    ::close(serverSocket->socket_handle_.fd_);
//  });
//
//  sockaddr_in addr;
//  addr.sin_family = AF_INET;
//  addr.sin_port = htons(_port);
//  ::inet_pton(addr.sin_family, _address.c_str(), &(addr.sin_addr));
//  if(::bind(kServerSocketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
//    perror("bind");
//    return nullptr;
//  }
//
//  ::listen(kServerSocketFd, _numQueue);
//
//  if (setSocketBlockingMode(kServerSocketFd, false)) {
//    socketPtr->setBlockingMode(false);
//  }
//  return socketPtr;
//}
//
//std::shared_ptr<ServerSocket::ServerSocketImpl> EventLoop::EventLoopImpl::createServerSocketImpl(const Handle & _socketHandle) {
//  if(_socketHandle.fd_ < 0) {
//    perror("socket");
//    return nullptr;
//  }
//
//  auto socketRawPtr = new ServerSocket::ServerSocketImpl(Handle{_socketHandle});
//  function_wrapper<void(const Handle&)> readCallback(&ServerSocket::ServerSocketImpl::onSocketDataAvailable, socketRawPtr);
//  registerObjectEvents(Handle{_socketHandle}, EventType::READ, readCallback);
//  auto socketPtr = std::shared_ptr<ServerSocket::ServerSocketImpl>(socketRawPtr,
//  [this, readCallback](ServerSocket::ServerSocketImpl* serverSocket) {
//    unregisterObjectEvents(serverSocket->socket_handle_, EventType::READ, readCallback);
//    ::close(serverSocket->socket_handle_.fd_);
//  });
//
//  if (setSocketBlockingMode(_socketHandle.fd_, false)) {
//    socketPtr->setBlockingMode(false);
//  }
//  return socketPtr;
//}
//
//std::shared_ptr<Socket::SocketImpl> EventLoop::EventLoopImpl::createSocketImpl(const std::string& _address, const uint16_t _port) {
//  const int kSocketFd = ::socket(AF_INET, SOCK_STREAM, 0);
//  if(kSocketFd < 0) {
//    perror("socket");
//    return nullptr;
//  }
//
//  auto socketRawPtr = new Socket::SocketImpl(Handle{kSocketFd});
//  function_wrapper<void(const Handle&)> readCallback(&Socket::SocketImpl::onSocketDataAvailable, socketRawPtr);
//  registerObjectEvents(Handle{kSocketFd}, EventType::READ, readCallback);
//  function_wrapper<void(const Handle&)> writeCallback(&Socket::SocketImpl::onSocketBufferAvailable, socketRawPtr);
//  registerObjectEvents(Handle{kSocketFd}, EventType::WRITE, writeCallback);
//  auto socketPtr = std::shared_ptr<Socket::SocketImpl>(socketRawPtr,
//  [this, readCallback, writeCallback](Socket::SocketImpl* socket) {
//    unregisterObjectEvents(socket->socket_handle_, EventType::READ, readCallback);
//    unregisterObjectEvents(socket->socket_handle_, EventType::WRITE, writeCallback);
//    ::close(socket->socket_handle_.fd_);
//  });
//
//  sockaddr_in addr;
//  addr.sin_family = AF_INET;
//  addr.sin_port = htons(_port);
//  ::inet_pton(addr.sin_family, _address.c_str(), &(addr.sin_addr));
//  if(::connect(kSocketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
//    perror("connect");
//    return nullptr;
//  }
//
//  if (setSocketBlockingMode(kSocketFd, false)) {
//    socketPtr->setBlockingMode(false);
//  }
//  return socketPtr;
//}
//
//std::shared_ptr<Socket::SocketImpl> EventLoop::EventLoopImpl::createSocketImpl(const Handle & _socketHandle) {
//  if(_socketHandle.fd_ < 0) {
//    perror("socket");
//    return nullptr;
//  }
//
//  auto socketRawPtr = new Socket::SocketImpl(_socketHandle);
//  function_wrapper<void(const Handle&)> readCallback(&Socket::SocketImpl::onSocketDataAvailable, socketRawPtr);
//  registerObjectEvents(_socketHandle, EventType::READ, readCallback);
//  function_wrapper<void(const Handle&)> writeCallback(&Socket::SocketImpl::onSocketBufferAvailable, socketRawPtr);
//  registerObjectEvents(_socketHandle, EventType::WRITE, writeCallback);
//  auto socketPtr = std::shared_ptr<Socket::SocketImpl>(socketRawPtr,
//  [this, readCallback, writeCallback](Socket::SocketImpl* socket) {
//    unregisterObjectEvents(socket->socket_handle_, EventType::READ, readCallback);
//    unregisterObjectEvents(socket->socket_handle_, EventType::WRITE, writeCallback);
//    ::close(socket->socket_handle_.fd_);
//  });
//
//  if (setSocketBlockingMode(_socketHandle.fd_, false)) {
//    socketPtr->setBlockingMode(false);
//  }
//  return socketPtr;
//}

bool EventLoop::EventLoopImpl::isRun() const {
  return execute_.load(std::memory_order_acquire);
}

DWORD WINAPI serverWorkerThread(LPVOID CompletionPortID) {
  HANDLE completionPort = static_cast<HANDLE>(CompletionPortID);
  DWORD bytesTransferred;
  LPPER_HANDLE_DATA perHandleData;
  LPPER_IO_OPERATION_DATA perIoData;
  DWORD sendBytes, recvBytes;
  DWORD flags;

  while(true) {
    if (GetQueuedCompletionStatus(completionPort,
                                  &bytesTransferred,
                                  (LPDWORD)&perHandleData,
                                  (LPOVERLAPPED *) &perIoData, INFINITE) == 0) {
      printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
      return 0;
    } else {
      printf("GetQueuedCompletionStatus() is OK!\n");
    }

    // First check to see if an error has occurred on the socket and if so
    // then close the socket and cleanup the SOCKET_INFORMATION structure
    // associated with the socket
    if (bytesTransferred == 0) {
      printf("Closing socket %d\n", perHandleData->socket);
      if (closesocket(perHandleData->socket) == SOCKET_ERROR) {
        printf("closesocket() failed with error %d\n", WSAGetLastError());
        return 0;
      } else {
        printf("closesocket() is fine!\n");
      }

      GlobalFree(perHandleData);
      GlobalFree(perIoData);
      continue;
    }

    // Check to see if the BytesRECV field equals zero. If this is so, then
    // this means a WSARecv call just completed so update the BytesRECV field
    // with the BytesTransferred value from the completed WSARecv() call
    if (perIoData->bytesRECV == 0) {
      perIoData->bytesRECV = bytesTransferred;
      perIoData->bytesSEND = 0;
    } else {
      perIoData->bytesSEND += bytesTransferred;
    }

    if (perIoData->bytesRECV > perIoData->bytesSEND) {
      // Post another WSASend() request.
      // Since WSASend() is not guaranteed to send all of the bytes requested,
      // continue posting WSASend() calls until all received bytes are sent.
      ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
      perIoData->dataBuf.buf = perIoData->buffer + perIoData->bytesSEND;
      perIoData->dataBuf.len = perIoData->bytesRECV - perIoData->bytesSEND;
      if (WSASend(perHandleData->socket, &(perIoData->dataBuf), 1, &sendBytes, 0,
                  &(perIoData->overlapped), NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() != ERROR_IO_PENDING) {
          printf("WSASend() failed with error %d\n", WSAGetLastError());
          return 0;
        }
      } else {
        printf("WSASend() is OK!\n");
      }
    } else {
      perIoData->bytesRECV = 0;
      // Now that there are no more bytes to send post another WSARecv() request
      flags = 0;
      ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
      perIoData->dataBuf.len = DATA_BUFSIZE;
      perIoData->dataBuf.buf = perIoData->buffer;

      if (WSARecv(perHandleData->socket, &(perIoData->dataBuf), 1, &recvBytes, &flags,
                  &(perIoData->overlapped), NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() != ERROR_IO_PENDING) {
          printf("WSARecv() failed with error %d\n", WSAGetLastError());
          return 0;
        }
      } else {
        printf("WSARecv() is OK!\n");
      }
    }
  }
}

void EventLoop::EventLoopImpl::run() {
//  event_loop_handle_.handle_ = ::_open(".", O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR);
//  if (event_loop_handle_.fd_ == -1) {
//    std::cerr << strerror(errno) << "\n";
//    THROW_OR_EXIT("Error !!";
//  }
  // Step 1:
  HANDLE ioCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr,0,0);

  // Step 2:
  // Determine how many processors are on the system
  SYSTEM_INFO systemInfo;
  ::GetSystemInfo(&systemInfo);
  for (int i = 0; i < systemInfo.dwNumberOfProcessors; i++) {
    HANDLE threadHandle;
    // Create a server worker thread, and pass the
    // completion port to the thread. NOTE: the
    // ServerWorkerThread procedure is not defined in this listing.
    threadHandle = ::CreateThread(nullptr, 0, serverWorkerThread, ioCompletionPort, 0, nullptr);
    // Close the thread handle
    ::CloseHandle(threadHandle);
  }


// Step 3:
// Create worker threads based on the number of
// processors available on the system. For this
// simple case, we create one worker thread for each processor.

  execute_.store(true, std::memory_order_release);
  {
    std::lock_guard<std::mutex> lock(internal_mtx_);
    addFdTo(lock, read_listeners_, add_read_listeners_);
    addFdTo(lock, write_listeners_, add_write_listeners_);
    addFdTo(lock, error_listeners_, add_error_listeners_);
    removeFdFrom(lock, read_listeners_, remove_read_listeners_);
    removeFdFrom(lock, write_listeners_, remove_write_listeners_);
    removeFdFrom(lock, error_listeners_, remove_error_listeners_);
  }
  while (execute_.load(std::memory_order_acquire)) {
    fd_set readFds;
    fd_set writeFds;
    fd_set errorFds;

    int maxFd = 0;
//    maxFd = std::max(maxFd, event_loop_handle_.fd_);
//    FD_SET(event_loop_handle_.fd_, &readFds);
    initFds(read_listeners_, readFds, maxFd);
    initFds(write_listeners_, writeFds, maxFd);
    initFds(error_listeners_, errorFds, maxFd);

    ::select(maxFd + 1, &readFds, &writeFds, &errorFds, nullptr);
//    if (!execute_.load(std::memory_order_acquire)) {
//      eventfd_write(event_loop_handle_.fd_, 0);
//      break;
//    }

    handleLoopEvents(readFds);
    handleHandlesEvents(read_listeners_, readFds);
    handleHandlesEvents(write_listeners_, writeFds);
    handleHandlesEvents(error_listeners_, errorFds);
  }
}

void EventLoop::EventLoopImpl::stop() {
//  if (event_loop_handle_.fd_ != -1) {
//    execute_.store(false, std::memory_order_release);
//    eventfd_write(event_loop_handle_.fd_, 1);
//  }
}

void EventLoop::EventLoopImpl::registerObjectEvents(
    const Handle &osObject,
    const EventType &eventType,
    function_wrapper<void(const Handle &)> callback) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  switch (eventType) {
    case EventType::kRead: {
      add_read_listeners_.emplace_back(osObject, callback);
      break;
    }
    case EventType::kWrite: {
      add_write_listeners_.emplace_back(osObject, callback);
      break;
    }
    case EventType::kError: {
      add_error_listeners_.emplace_back(osObject, callback);
      break;
    }
  }
//  eventfd_t updated = 1;
//  eventfd_write(event_loop_handle_.fd_, updated);
}

void EventLoop::EventLoopImpl::unregisterObjectEvents(
    const Handle &osObject,
    const EventType &eventType,
    function_wrapper<void(const Handle &)> callback) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  switch (eventType) {
    case EventType::kRead: {
      remove_read_listeners_.emplace_back(osObject, callback);
      break;
    }
    case EventType::kWrite: {
      remove_write_listeners_.emplace_back(osObject, callback);
      break;
    }
    case EventType::kError: {
      remove_error_listeners_.emplace_back(osObject, callback);
      break;
    }
  }
//  eventfd_t updated = 1;
//  eventfd_write(event_loop_handle_.fd_, updated);
}

void EventLoop::EventLoopImpl::addFdTo(std::lock_guard<std::mutex> &lock,
                                       std::vector<HandleListeners> &listeners,
                                       const std::vector<InternalEvent> &addListeners) {
  if (!addListeners.empty()) {
    for (auto &fdInfo : addListeners) {
      auto foundFd = findOSObjectIn(fdInfo.object_, listeners);
      if (foundFd != listeners.end()) {
        foundFd->callbacks_.push_back(fdInfo.callback_);
        auto erased = std::unique(foundFd->callbacks_.begin(), foundFd->callbacks_.end());
        foundFd->callbacks_.erase(erased, foundFd->callbacks_.end());
      } else {
        listeners.emplace_back(fdInfo.object_, std::vector<function_wrapper<void(const Handle &)>>{fdInfo.callback_});
      }
    }
  }
}

void EventLoop::EventLoopImpl::removeFdFrom(std::lock_guard<std::mutex> &lock,
                                            std::vector<HandleListeners> &listeners,
                                            const std::vector<InternalEvent> &removeListeners) {
  if (!removeListeners.empty()) {
    for (auto &fdInfo : removeListeners) {
      auto foundFd = findOSObjectIn(fdInfo.object_, listeners);
      if (foundFd != listeners.end()) {
        auto itemToRemove = std::remove(foundFd->callbacks_.begin(), foundFd->callbacks_.end(), fdInfo.callback_);
        foundFd->callbacks_.erase(itemToRemove);
        if (foundFd->callbacks_.empty()) {
          listeners.erase(foundFd);
        }
      }
    }
  }
}

void EventLoop::EventLoopImpl::initFds(std::vector<HandleListeners> &fds,
                                       fd_set &fdSet,
                                       int &maxFd) const {
//  for (const auto &fdInfo : fds) {
//    FD_SET(fdInfo.handle_.fd_, &fdSet);
//    if (fdInfo.handle_.fd_ > maxFd) {
//      maxFd = fdInfo.handle_.fd_;
//    }
//  }
}

void EventLoop::EventLoopImpl::handleLoopEvents(fd_set fdSet) {
  if (FD_ISSET(event_loop_handle_.handle_, &fdSet)) {
    std::lock_guard<std::mutex> lock(internal_mtx_);
//    eventfd_t updatedEvent;
//    eventfd_read(event_loop_handle_.fd_, &updatedEvent);
//    if (updatedEvent > 0) {
      addFdTo(lock, read_listeners_, add_read_listeners_);
      addFdTo(lock, write_listeners_, add_write_listeners_);
      addFdTo(lock, error_listeners_, add_error_listeners_);
      removeFdFrom(lock, read_listeners_, remove_read_listeners_);
      removeFdFrom(lock, write_listeners_, remove_write_listeners_);
      removeFdFrom(lock, error_listeners_, remove_error_listeners_);
//      eventfd_write(event_loop_handle_.handle_, 0);
//    }
  }
}

void EventLoop::EventLoopImpl::handleHandlesEvents(std::vector<HandleListeners> &fds, fd_set &fdSet) {
  for (const auto &fdInfo : fds) {
    if (FD_ISSET(fdInfo.handle_.handle_, &fdSet)) {
      for (const auto &callback : fdInfo.callbacks_) {
        callback(fdInfo.handle_);
      }
    }
  }
}

std::vector<EventLoop::EventLoopImpl::HandleListeners>::iterator
EventLoop::EventLoopImpl::findOSObjectIn(const Handle &osObject, std::vector<HandleListeners> &fds) {
  auto foundFd = std::find_if(fds.begin(), fds.end(),
                              [osObject](const HandleListeners &fdInfo) {
                                return fdInfo.handle_.handle_ == osObject.handle_;
                              });
  return foundFd;
}

}

}
