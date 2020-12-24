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

#include "Common.hpp"
#include "EventLoopImpl.hpp"

namespace icc {

namespace os {

typedef struct _PER_HANDLE_DATA
{
  SOCKET        Socket;
  SOCKADDR_STORAGE  ClientAddr;
  // Other information useful to be associated with the handle
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

EventLoop::EventLoopImpl::EventLoopImpl(std::nullptr_t)
    : EventLoopImpl() {
  // Step 1:
  int iResult = ::WSAStartup(MAKEWORD(2, 2), &wsa_data_);
  if (iResult != NO_ERROR) {
    wprintf(L"Error at WSAStartup()\n");
    throw std::runtime_error("WSAStartup is failed");
  }
  io_completion_port_ = Handle{::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr,0,0)};
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
//  const int kTimerFd = 0;
//  auto timer = new Timer::TimerImpl(Handle{kTimerFd});
//  function_wrapper<void(const Handle &)> callback(&Timer::TimerImpl::onTimerExpired, timer);
//  registerObjectEvents(Handle{kTimerFd}, EventType::kRead, callback);
//  return std::shared_ptr<Timer::TimerImpl>(timer,
//                                           [this, callback](Timer::TimerImpl *timer) {
//                                             unregisterObjectEvents(timer->timer_handle_, EventType::kRead, callback);
//                                             //TODO(redradist): Timer should be closed
//                                           });
  return nullptr;
}

bool
EventLoop::EventLoopImpl::setSocketBlockingMode(SOCKET _fd, bool _isBlocking) {
  if (_fd < 0) {
    return false;
  }

  unsigned long mode = _isBlocking ? 0 : 1;
  return (::ioctlsocket(_fd, FIONBIO, &mode) == 0);
}

std::shared_ptr<ServerSocket::ServerSocketImpl>
EventLoop::EventLoopImpl::createServerSocketImpl(std::string _address, uint16_t _port, uint16_t _numQueue) {
  const SOCKET kServerSocketFd = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
  if(kServerSocketFd < 0) {
    perror("socket");
    return nullptr;
  }

  auto socketRawPtr = new ServerSocket::ServerSocketImpl(Handle{reinterpret_cast<HANDLE>(kServerSocketFd)}, io_completion_port_);
  auto socketPtr = std::shared_ptr<ServerSocket::ServerSocketImpl>(socketRawPtr,
  [this, kServerSocketFd](ServerSocket::ServerSocketImpl* serverSocket) {
    ::closesocket(kServerSocketFd);
  });

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(_port);
  if(SOCKET_ERROR == ::bind(kServerSocketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))) {
    auto err = WSAGetLastError();
    wprintf(L"bind failed with error %u\n", err);
    return nullptr;
  }

  ::listen(kServerSocketFd, _numQueue);

  if (setSocketBlockingMode(kServerSocketFd, false)) {
    socketPtr->setBlockingMode(false);
  }
  return socketPtr;
}

std::shared_ptr<ServerSocket::ServerSocketImpl>
EventLoop::EventLoopImpl::createServerSocketImpl(const Handle & _socketHandle) {
  if(_socketHandle.handle_ < 0) {
    perror("socket");
    return nullptr;
  }

  auto socketRawPtr = new ServerSocket::ServerSocketImpl(_socketHandle, io_completion_port_);
  auto socketPtr = std::shared_ptr<ServerSocket::ServerSocketImpl>(socketRawPtr,
  [this, _socketHandle](ServerSocket::ServerSocketImpl* serverSocket) {
    ::closesocket(reinterpret_cast<SOCKET>(_socketHandle.handle_));
  });

  if (setSocketBlockingMode(reinterpret_cast<SOCKET>(_socketHandle.handle_), false)) {
    socketPtr->setBlockingMode(false);
  }
  return socketPtr;
}

std::shared_ptr<Socket::SocketImpl>
EventLoop::EventLoopImpl::createSocketImpl(const std::string& _address, const uint16_t _port) {
  const SOCKET kSocketFd = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
  if(kSocketFd < 0) {
    perror("socket");
    return nullptr;
  }

  auto socketRawPtr = new Socket::SocketImpl(Handle{reinterpret_cast<HANDLE>(kSocketFd)});
  function_wrapper<void(const Handle&)> readCallback(&Socket::SocketImpl::onSocketDataAvailable, socketRawPtr);
  function_wrapper<void(const Handle&)> writeCallback(&Socket::SocketImpl::onSocketBufferAvailable, socketRawPtr);
  auto socketPtr = std::shared_ptr<Socket::SocketImpl>(socketRawPtr,
  [this, readCallback, writeCallback](Socket::SocketImpl* socket) {
    unregisterObjectEvents(socket->socket_handle_, FD_READ, readCallback);
    unregisterObjectEvents(socket->socket_handle_, FD_WRITE, writeCallback);
    ::closesocket(reinterpret_cast<SOCKET>(socket->socket_handle_.handle_));
  });

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_port);
  if(::connect(kSocketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    perror("connect");
    return nullptr;
  }

  if (setSocketBlockingMode(kSocketFd, false)) {
    socketPtr->setBlockingMode(false);
  }
  return socketPtr;
}

std::shared_ptr<Socket::SocketImpl> EventLoop::EventLoopImpl::createSocketImpl(const Handle & _socketHandle) {
  PER_HANDLE_DATA *PerHandleData = nullptr;
  SOCKADDR_IN saRemote;
  SOCKET Accept;
  int RemoteLen;

  if(_socketHandle.handle_ < 0) {
    perror("socket");
    return nullptr;
  }

  // Step 6:
  // Create per-handle data information structure to associate with the socket
  RemoteLen = sizeof(saRemote);
  PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
  printf("Socket number %d connected\n", Accept);
  PerHandleData->Socket = Accept;
  ::memcpy(&PerHandleData->ClientAddr, &saRemote, RemoteLen);
  // Step 7:
  // Associate the accepted socket with the completion port
  ::CreateIoCompletionPort((HANDLE) Accept, io_completion_port_.handle_, (DWORD) PerHandleData, 0);

  auto socketRawPtr = new Socket::SocketImpl(_socketHandle);
  function_wrapper<void(const Handle&)> readCallback(&Socket::SocketImpl::onSocketDataAvailable, socketRawPtr);
  registerObjectEvents(_socketHandle, FD_READ, readCallback);
  function_wrapper<void(const Handle&)> writeCallback(&Socket::SocketImpl::onSocketBufferAvailable, socketRawPtr);
  registerObjectEvents(_socketHandle, FD_WRITE, writeCallback);
  auto socketPtr = std::shared_ptr<Socket::SocketImpl>(socketRawPtr,
  [this, readCallback, writeCallback](Socket::SocketImpl* socket) {
    unregisterObjectEvents(socket->socket_handle_, FD_READ, readCallback);
    unregisterObjectEvents(socket->socket_handle_, FD_WRITE, writeCallback);
    ::closesocket(reinterpret_cast<SOCKET>(socket->socket_handle_.handle_));
  });

  if (setSocketBlockingMode(reinterpret_cast<SOCKET>(_socketHandle.handle_), false)) {
    socketPtr->setBlockingMode(false);
  }
  return socketPtr;
}

bool EventLoop::EventLoopImpl::isRun() const {
  return execute_.load(std::memory_order_acquire);
}

void EventLoop::EventLoopImpl::run() {
  event_loop_handle_.handle_ = CreateEvent(
      nullptr,                        // default security attributes
      TRUE,                           // manual-reset event
      FALSE,                          // initial state is nonsignaled
      TEXT("AddRemoveEvent")    // object name
  );
  if (event_loop_handle_.handle_ == INVALID_HANDLE_VALUE) {
    std::cerr << strerror(errno) << "\n";
    throw "Error to create CreateEvent(...) !!";
  }
  execute_.store(true, std::memory_order_release);
  WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
  eventArray[0] = event_loop_handle_.handle_;
  while (execute_.load(std::memory_order_acquire)) {
    initFds(event_listeners_, &eventArray[1], WSA_MAXIMUM_WAIT_EVENTS-1);

    DWORD event;
    if ((event = ::WSAWaitForMultipleEvents(event_listeners_.size(), eventArray, FALSE, WSA_INFINITE, FALSE)) == WSA_WAIT_FAILED) {
      printf("WSAWaitForMultipleEvents() failed with error %d\n", WSAGetLastError());
    } else {
      printf("WSAWaitForMultipleEvents() is pretty damn OK!\n");
    }

    handleLoopEvents(eventArray[event - WSA_WAIT_EVENT_0]);
    handleHandlesEvents(event_listeners_, eventArray[event - WSA_WAIT_EVENT_0]);
  }
}

void EventLoop::EventLoopImpl::stop() {
  if (event_loop_handle_ != kInvalidHandle) {
    execute_.store(false, std::memory_order_release);
  }
}

void EventLoop::EventLoopImpl::registerObjectEvents(
    const Handle &osObject,
    const long event,
    function_wrapper<void(const Handle &)> callback) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  if (event_loop_handle_.handle_ != INVALID_HANDLE_VALUE) {
    add_event_listeners_.emplace_back(osObject, event, callback);
    ::SetEvent(event_loop_handle_.handle_);
  }
}

void EventLoop::EventLoopImpl::unregisterObjectEvents(
    const Handle &osObject,
    const long event,
    function_wrapper<void(const Handle &)> callback) {
  std::lock_guard<std::mutex> lock(internal_mtx_);
  if (event_loop_handle_.handle_ != INVALID_HANDLE_VALUE) {
    remove_event_listeners_.emplace_back(osObject, event, callback);
    ::SetEvent(event_loop_handle_.handle_);
  }
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
        WSAEVENT eventObject = ::WSACreateEvent();
        if (eventObject == WSA_INVALID_EVENT) {
          printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
          return;
        }
        if (::WSAEventSelect(reinterpret_cast<SOCKET>(fdInfo.object_.handle_), eventObject, fdInfo.event_) == SOCKET_ERROR){
          printf("WSAEventSelect() failed with error %d\n", WSAGetLastError());
          return;
        } else {
          printf("WSAEventSelect() is pretty fine!\n");
        }
        listeners.emplace_back(fdInfo.object_, eventObject, std::vector<function_wrapper<void(const Handle &)>>{fdInfo.callback_});
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
                                       WSAEVENT *eventArray,
                                       size_t numEvents) const {
  unsigned i = 0;
  for (const auto &fdInfo : fds) {
    if (i >= numEvents) {
      break;
    }
    eventArray[i++] = fdInfo.event_;
  }
}

void EventLoop::EventLoopImpl::handleLoopEvents(const WSAEVENT eventObj) {
  if (event_loop_handle_.handle_ == eventObj) {
    std::lock_guard<std::mutex> lock(internal_mtx_);
    ::ResetEvent(event_loop_handle_.handle_);
    addFdTo(lock, event_listeners_, add_event_listeners_);
    removeFdFrom(lock, event_listeners_, remove_event_listeners_);
  }
}

void EventLoop::EventLoopImpl::handleHandlesEvents(std::vector<HandleListeners> &fds, WSAEVENT event) {
  for (const auto &fdInfo : fds) {
    if (event == fdInfo.event_) {
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

// TODO(redradist): Use this as example for CompletionPort
//DWORD WINAPI socketsWorkerThread(LPVOID completionPortId) {
//  auto* params = static_cast<SocketsWorkerThreadParams*>(completionPortId);
//  DWORD bytesTransferred;
//  LpperHandleData perHandleData;
//  LpperIOOperationData perIoData;
//  DWORD sendBytes, recvBytes;
//  DWORD flags;
//
//  while(params->execute_.load(std::memory_order_acquire)) {
//    if (::GetQueuedCompletionStatus(params->io_completion_port_.handle_,
//                                    &bytesTransferred,
//                                    (PULONG_PTR)&perHandleData,
//                                    (LPOVERLAPPED *) &perIoData, INFINITE) == 0) {
//      printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
//      return 0;
//    } else {
//      printf("GetQueuedCompletionStatus() is OK!\n");
//    }
//
//    // First check to see if an error has occurred on the socket and if so
//    // then close the socket and cleanup the SOCKET_INFORMATION structure
//    // associated with the socket
//    if (bytesTransferred == 0) {
//      printf("Closing socket %lld\n", perHandleData->socket);
//      if (::closesocket(perHandleData->socket) == SOCKET_ERROR) {
//        printf("closesocket() failed with error %d\n", WSAGetLastError());
//        return 0;
//      } else {
//        printf("closesocket() is fine!\n");
//      }
//
//      ::GlobalFree(perHandleData);
//      ::GlobalFree(perIoData);
//      continue;
//    }
//
//    // Check to see if the BytesRECV field equals zero. If this is so, then
//    // this means a WSARecv call just completed so update the BytesRECV field
//    // with the BytesTransferred value from the completed WSARecv() call
//    if (perIoData->bytesRecv == 0) {
//      perIoData->bytesRecv = bytesTransferred;
//      perIoData->bytesSend = 0;
//    } else {
//      perIoData->bytesSend += bytesTransferred;
//    }
//
//    if (perIoData->bytesRecv > perIoData->bytesSend) {
//      // Post another WSASend() request.
//      // Since WSASend() is not guaranteed to send all of the bytes requested,
//      // continue posting WSASend() calls until all received bytes are sent.
//      ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
//      perIoData->dataBuf.buf = perIoData->buffer + perIoData->bytesSend;
//      perIoData->dataBuf.len = perIoData->bytesRecv - perIoData->bytesSend;
//      if (::WSASend(perHandleData->socket, &(perIoData->dataBuf), 1, &sendBytes, 0,
//                    &(perIoData->overlapped), nullptr) == SOCKET_ERROR) {
//        if (::WSAGetLastError() != ERROR_IO_PENDING) {
//          printf("WSASend() failed with error %d\n", WSAGetLastError());
//          return 0;
//        }
//      } else {
//        printf("WSASend() is OK!\n");
//      }
//    } else {
//      perIoData->bytesRecv = 0;
//      // Now that there are no more bytes to send post another WSARecv() request
//      flags = 0;
//      ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
//      perIoData->dataBuf.len = DATA_BUFF_SIZE;
//      perIoData->dataBuf.buf = perIoData->buffer;
//
//      if (::WSARecv(perHandleData->socket, &(perIoData->dataBuf), 1, &recvBytes, &flags,
//                    &(perIoData->overlapped), nullptr) == SOCKET_ERROR) {
//        if (::WSAGetLastError() != ERROR_IO_PENDING) {
//          printf("WSARecv() failed with error %d\n", WSAGetLastError());
//          return 0;
//        }
//      } else {
//        printf("WSARecv() is OK!\n");
//      }
//    }
//  }
//}

}

}
