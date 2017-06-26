//
// Created by redra on 25.06.17.
//

#ifndef ICC_PROCESSBUS_HPP
#define ICC_PROCESSBUS_HPP

#include <typeinfo>
#include <set>
#include <map>
#include "IService.hpp"
#include "IClient.hpp"

class ProcessBus {
 public:



  template <typename _Interface>
  void registerService(IService<_Interface> *) {
    typeid(IService<_Interface>);
  }

  template <typename _Interface>
  void buildClient(IClient<_Interface> *) {
    typeid(IClient<_Interface>);
  }

 public:
  static ProcessBus & getBus();

 private:
  ProcessBus() = default;
  ProcessBus(const ProcessBus &) = delete;
  ProcessBus(ProcessBus &&) = delete;

 private:
};

#endif //ICC_PROCESSBUS_HPP
