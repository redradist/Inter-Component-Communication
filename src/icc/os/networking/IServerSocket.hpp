//
// Created by redra on 11.12.19.
//

#ifndef ICC_ISERVERSOCKET_HPP
#define ICC_ISERVERSOCKET_HPP

#include <memory>
#include "Socket.hpp"

namespace icc {

namespace os {

class IServerSocket {
 public:
  virtual std::shared_ptr<Socket> accept() = 0;
  virtual std::future<std::shared_ptr<Socket>> acceptAsync() = 0;
  virtual const std::vector<std::shared_ptr<Socket>>& getClientSockets() const = 0;
};

}

}

#endif //ICC_ISERVERSOCKET_HPP
