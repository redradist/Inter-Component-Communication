//
// Created by redra on 17.11.19.
//

#ifndef ICC_ISOCKET_HPP
#define ICC_ISOCKET_HPP

#include <utility>
#include <future>

#include "SocketTypes.hpp"
#include "ISocketListener.hpp"

namespace icc {

namespace os {

class ISocket {
 public:
  virtual void send(ChunkData _data) = 0;
  virtual std::future<void> sendAsync(ChunkData _data) = 0;
  virtual ChunkData receive() = 0;
  virtual std::future<ChunkData> receiveAsync() = 0;
};

}

}

#endif //ICC_ISOCKET_HPP
