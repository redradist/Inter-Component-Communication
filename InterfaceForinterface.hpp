//
// Created by redra on 25.06.17.
//

#ifndef ICC_INTERFACEFORINTERFACE_HPP
#define ICC_INTERFACEFORINTERFACE_HPP

#include <functional>
#include "src/Event.hpp"

class InterfaceForInterface {
 public:
  virtual void addVersion(std::function<void(std::string)>) = 0;
  virtual void addVersion2() = 0;

  Event<void(const int &)> event_;
};

#endif //ICC_INTERFACEFORINTERFACE_HPP
