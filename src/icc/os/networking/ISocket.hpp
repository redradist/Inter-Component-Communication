//
// Created by redra on 17.11.19.
//

#ifndef ICC_ISOCKET_HPP
#define ICC_ISOCKET_HPP

#include "ISocketListener.hpp"

namespace icc {

namespace os {

class ISocket {
 public:
  virtual void send(std::vector<uint8_t> _data) = 0;
  virtual void sendAsync(std::vector<uint8_t> _data, ISocketSender &_sender) = 0;
  virtual void receive() = 0;
  virtual void receiveAsync(ISocketReceiver & _receiver) = 0;

  virtual void addListener(std::shared_ptr<ISocketListener> _listener) = 0;
  virtual void addListener(ISocketListener * _listener) = 0;
  virtual void removeListener(std::shared_ptr<ISocketListener> _listener) = 0;
  virtual void removeListener(ISocketListener * _listener) = 0;
};

}

}

#endif //ICC_ISOCKET_HPP
