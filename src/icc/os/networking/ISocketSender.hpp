//
// Created by redra on 16.11.19.
//

#ifndef ICC_ISOCKETSENDER_HPP
#define ICC_ISOCKETSENDER_HPP

namespace icc {

namespace os {

class ISocketSender {
 public:
  virtual void onDataSent(const std::vector<uint8_t> & _data) = 0;
};

}

}

#endif //ICC_ISOCKETSENDER_HPP