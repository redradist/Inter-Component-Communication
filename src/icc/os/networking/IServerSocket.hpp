//
// Created by redra on 11.12.19.
//

#ifndef ICC_ISERVERSOCKET_HPP
#define ICC_ISERVERSOCKET_HPP

#include <memory>
#include "IServerSocketListener.hpp"

namespace icc {

namespace os {

class IServerSocket {
 public:
  virtual void addListener(std::shared_ptr<IServerSocketListener> _listener) = 0;
  virtual void addListener(IServerSocketListener * _listener) = 0;
  virtual void removeListener(std::shared_ptr<IServerSocketListener> _listener) = 0;
  virtual void removeListener(IServerSocketListener * _listener) = 0;
  virtual const std::vector<std::shared_ptr<Socket>>& getClientSockets() const = 0;
};

}

}

#endif //ICC_ISERVERSOCKET_HPP
