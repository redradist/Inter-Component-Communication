//
// Created by redra on 18.07.19.
//

#include "SocketImpl.hpp"

namespace icc {

namespace os {

Socket::SocketImpl::SocketImpl(const Handle & socketObject)
    : socket_object_{socketObject} {
}

void Socket::SocketImpl::send(std::vector<uint8_t> _data) {

}

void Socket::SocketImpl::sendAsync(std::vector<uint8_t> _data,
                                   ISocketSender &_sender) {

}

void Socket::SocketImpl::receive() {

}

void Socket::SocketImpl::receiveAsync(ISocketReceiver & _receiver) {

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