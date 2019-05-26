//
// Created by redra on 26.05.19.
//

#ifndef ICC_ICHANNEL_HPP
#define ICC_ICHANNEL_HPP

#include <functional>

namespace icc {

using Action = std::function<void(void)>;

class IChannel {
 public:
  virtual void push(Action _action) = 0;
  virtual void invoke(Action _action) = 0;
};

}

#endif //ICC_ICHANNEL_HPP
