//
// Created by redra on 11.12.19.
//

#ifndef ICC_ISERVERSOCKET_HPP
#define ICC_ISERVERSOCKET_HPP

namespace icc {

namespace os {

class IServerSocket {
 public:
  virtual std::shared_ptr<ISocket> acceptConnection() = 0;
  virtual std::future<SharedChunkData> listen() = 0;
};

}

}

#endif //ICC_ISERVERSOCKET_HPP
