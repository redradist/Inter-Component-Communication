//
// Created by redra on 25.06.17.
//

#ifndef ICC_SERVICE_HPP
#define ICC_SERVICE_HPP

#include <type_traits>

template <typename _Interface>
class IService
  : public _Interface {
  static_assert(std::is_abstract<_Interface>::value,
                "_Interface is not an abstract class");
 public:
  IService() {

  }

  auto operator->() {

  }
};

#endif //ICC_SERVICE_HPP
