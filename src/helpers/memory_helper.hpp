//
// Created by redra on 05.07.17.
//

#ifndef ICC_HELPER_HPP
#define ICC_HELPER_HPP

#include <memory>

/**
 * Header with helper classes and functions for <memory>
 * from standard library
 */
namespace std {

class virtual_enable_shared_from_this_base
    : public std::enable_shared_from_this<virtual_enable_shared_from_this_base> {
 public:
  virtual ~virtual_enable_shared_from_this_base() {}
};

template <typename _T>
class virtual_enable_shared_from_this
    : public virtual virtual_enable_shared_from_this_base {
 public:
  std::shared_ptr<_T> shared_from_this() {
    return std::dynamic_pointer_cast<_T>(
        virtual_enable_shared_from_this_base::shared_from_this());
  }
};

}

#endif //ICC_HELPER_HPP
