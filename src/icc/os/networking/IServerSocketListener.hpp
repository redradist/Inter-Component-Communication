//
// Created by redra on 05.04.20.
//

#ifndef ICC_ISERVERSOCKETLISTENER_HPP
#define ICC_ISERVERSOCKETLISTENER_HPP

#include "Socket.hpp"

namespace icc {

namespace os {

class IServerSocketListener {
 public:
  virtual void onNewClientSocket(std::shared_ptr<Socket> _client) = 0;
};

}

}

#endif //ICC_ISERVERSOCKETLISTENER_HPP
