//
// Created by redra on 25.06.17.
//

#ifndef ICC_INTERFACEFORINTERFACE_HPP
#define ICC_INTERFACEFORINTERFACE_HPP

#include <functional>

class InterfaceForInterface {
 public:
  virtual void addVersion(std::function<void(std::string)>) = 0;
  virtual void addVersion2() = 0;
};

#endif //ICC_INTERFACEFORINTERFACE_HPP
