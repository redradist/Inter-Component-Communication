//
// Created by redra on 16.11.19.
//

#ifndef ICC_ISOCKETRECEIVER_HPP
#define ICC_ISOCKETRECEIVER_HPP

#include <vector>

#include "SocketTypes.hpp"

namespace icc {

namespace os {

class ISocketReceiver {
 public:
  virtual void onDataReceived(SharedChunkData _data) = 0;
};

}

}

#endif //ICC_ISOCKETRECEIVER_HPP
