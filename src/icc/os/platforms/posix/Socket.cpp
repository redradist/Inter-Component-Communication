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

std::shared_ptr<Socket> Socket::createSocket(const std::string _address, const uint16_t _port) {
  return EventLoop::getDefaultInstance().createSocket(_address, _port);
}

void Socket::send(std::vector<uint8_t> _data) {
  impl_ptr_->send(_data);
}

std::future<void> Socket::sendAsync(std::vector<uint8_t> _data, ISocketSender &_sender) {
  return impl_ptr_->sendAsync(_data, _sender);
}

ChunkData Socket::receive() {
  return impl_ptr_->receive();
}

std::future<ChunkData> Socket::receiveAsync(ISocketReceiver & _receiver) {
  return impl_ptr_->receiveAsync(_receiver);
}

void Socket::addListener(std::shared_ptr<ISocketListener> _listener) {
  impl_ptr_->addListener(_listener);
}

void Socket::addListener(ISocketListener * _listener) {
  impl_ptr_->addListener(_listener);
}

void Socket::removeListener(std::shared_ptr<ISocketListener> _listener) {
  impl_ptr_->removeListener(_listener);
}

void Socket::removeListener(ISocketListener * _listener) {
  impl_ptr_->removeListener(_listener);
}

}

}
