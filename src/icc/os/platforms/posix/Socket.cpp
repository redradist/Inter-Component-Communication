//
// Created by redra on 18.07.19.
//

#include <icc/os/EventLoop.hpp>
#include <icc/os/networking/Socket.hpp>

#include "SocketImpl.hpp"

namespace icc {

namespace os {

Socket::Socket(std::shared_ptr<SocketImpl> implPtr)
  : impl_ptr_{implPtr} {
}

std::shared_ptr<Socket> Socket::createSocket() {
  return EventLoop::getDefaultInstance().createSocket();
}

void Socket::send(std::vector<uint8_t> _data) {
  impl_ptr_->send(_data);
}

void Socket::sendAsync(std::vector<uint8_t> _data, ISocketSender &_sender) {
  impl_ptr_->sendAsync(_data, _sender);
}

void Socket::receive() {
  impl_ptr_->receive();
}

void Socket::receiveAsync(ISocketReceiver & _receiver) {
  impl_ptr_->receiveAsync(_receiver);
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
