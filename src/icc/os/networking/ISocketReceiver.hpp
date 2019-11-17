//
// Created by redra on 16.11.19.
//

#ifndef ICC_ISOCKETRECEIVER_HPP
#define ICC_ISOCKETRECEIVER_HPP

#include <vector>

namespace icc {

namespace os {

class ISocketReceiver {
 public:
  virtual void onDataReceived(const std::vector<uint8_t> & _data) = 0;
};

}

}

#endif //ICC_ISOCKETRECEIVER_HPP
