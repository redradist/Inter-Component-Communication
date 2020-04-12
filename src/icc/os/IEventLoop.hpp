//
// Created by redra on 12.04.20.
//

#ifndef OPTIONAL_IEVENTLOOP_HPP
#define OPTIONAL_IEVENTLOOP_HPP

namespace icc {

namespace os {

class IEventLoop {
 public:
  virtual void run() = 0;
  virtual void stop() = 0;
  virtual bool isRun() const = 0;
};

}

}

#endif //OPTIONAL_IEVENTLOOP_HPP
