//
// Created by redra on 25.06.17.
//

#ifndef ICC_ISERVICECLIENT_HPP
#define ICC_ISERVICECLIENT_HPP

#include <type_traits>
#include <iostream>
#include "../IComponent.hpp"

class IClientHelper {
 public:
  template <typename ... _Args>
  auto operator()(_Args ... args) {
    std::cout << "Helper was called" << std::endl;
  }

  template <typename ... _Args>
  auto operator*(_Args ... args) {
    std::cout << "Helper was called" << std::endl;
  }
};

template <typename _Interface>
class IClient
  : public IComponent,
    public _Interface {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  IClient() {
  }

  auto operator->() {
    std::cout << "Wrapper was called" << std::endl;
//    using expander = int[];
//    std::cout << "Wrapper was before" << std::endl;
//    (void)expander{0, (void(std::cout << ',' << typeid(_Args).name()),0)...};
//    std::cout << "Wrapper was after" << std::endl;
    return &client;
  }

  IClientHelper client;

};

#endif //ICC_ISERVICECLIENT_HPP
