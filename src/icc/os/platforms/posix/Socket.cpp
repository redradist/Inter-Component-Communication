//
// Created by redra on 18.07.19.
//

#include <utility>

#include <icc/os/EventLoop.hpp>
#include <icc/os/networking/Socket.hpp>

#include "SocketImpl.hpp"

namespace icc {

namespace os {

Socket::Socket(std::shared_ptr<SocketImpl> implPtr)
  : impl_ptr_{std::move(implPtr)} {
}

std::shared_ptr<ISocket> Socket::createSocket(const std::string _address, const uint16_t _port) {
  return EventLoop::getDefaultInstance().createSocket(_address, _port);
}

void Socket::send(std::vector<uint8_t> _data) {
  return impl_ptr_->send(_data);
}

std::future<void>
Socket::sendAsync(std::vector<uint8_t> _data) {
  return impl_ptr_->sendAsync(_data);
}

ChunkData Socket::receive() {
  return impl_ptr_->receive();
}

std::future<ChunkData> Socket::receiveAsync() {
  return impl_ptr_->receiveAsync();
}

}

}
