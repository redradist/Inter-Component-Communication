/**
 * @file memory_helper.hpp
 * @author Denis Kotov
 * @date 05 Jul 2017
 * @brief Contains helper functions and classes for <memory>
 * header from standard library
 * @copyright Denis Kotov, MIT License. Open source:
 */

#ifndef ICC_MEMORY_HELPER_HPP
#define ICC_MEMORY_HELPER_HPP

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

#endif //ICC_MEMORY_HELPER_HPP
