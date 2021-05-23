/**
 * @file Optional.hpp
 * @author Denis Kotov
 * @date 11 Apr 2020
 * @brief Contains Optional class.
 * It is implementation of std::optional<T> for pre-C++17 compilers that has compatible interface
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_OPTIONAL_HPP
#define ICC_OPTIONAL_HPP

#if __cpp_lib_optional >= 201606L
#include <optional>
#endif

#include <icc/_private/containers/exceptions/ContainerError.hpp>

namespace icc {

namespace _private {

namespace containers {

#if __cpp_constexpr >= 201304L
#  define CONSTEXPR constexpr
#else
#  define CONSTEXPR inline
#endif

template<typename T>
class Optional {
 public:
  CONSTEXPR Optional() = default;
  CONSTEXPR Optional(const T &_other)
      : is_present_{true}, value_{_other} {
  }
  CONSTEXPR Optional(T &&_other) noexcept
      : is_present_{true}, value_{std::move(_other)} {
  }
  CONSTEXPR Optional(const Optional &_other)
      : is_present_{_other.is_present_}, value_{_other.value_} {
  }
  CONSTEXPR Optional(Optional &&_other) noexcept
      : is_present_{std::move(_other.is_present_)}, value_{std::move(_other.value_)} {
  }
#if __cpp_lib_optional >= 201606L
  CONSTEXPR Optional(const std::optional<T> & _other)
    : is_present_{_other.has_value()}
    , value_{_other.has_value() ? _other.value() : T{}} {
  }
  CONSTEXPR Optional(std::optional<T>&& _other) noexcept
    : is_present_{_other.has_value()}
    , value_{std::move(_other.has_value() ? _other.value() : T{})} {
  }
  CONSTEXPR operator std::optional<T>() const noexcept {
    if (!is_present_) {
      return std::nullopt;
    }
    return std::optional<T>{value_};
  }
#endif
  CONSTEXPR Optional &operator=(const T &_other) {
    is_present_ = true;
    value_ = _other;
  }
  CONSTEXPR Optional &operator=(T &&_other) noexcept {
    is_present_ = true;
    value_ = std::move(_other);
  }
  CONSTEXPR Optional &operator=(const Optional &_other) {
    is_present_ = _other.is_present_;
    value_ = _other.value_;
  }
  CONSTEXPR Optional &operator=(Optional &&_other) noexcept {
    is_present_ = std::move(_other.is_present_);
    value_ = std::move(_other.value_);
  }
  CONSTEXPR explicit operator bool() const noexcept {
    return has_value();
  }
#if __cpp_lib_optional >= 201606L
  [[nodiscard]]
#endif
  CONSTEXPR bool has_value() const noexcept {
    return is_present_;
  }
  CONSTEXPR T &value() &{
    if (!is_present_) {
      throw icc::_private::containers::ContainerError("No value !!");
    }
    return value_;
  }
  CONSTEXPR const T &value() const &{
    if (!is_present_) {
      throw icc::_private::containers::ContainerError("No value !!");
    }
    return value_;
  }
  CONSTEXPR T &&value() &&{
    if (!is_present_) {
      throw icc::_private::containers::ContainerError("No value !!");
    }
    return std::move(value_);
  }
  CONSTEXPR const T &&value() const &&{
    if (!is_present_) {
      throw icc::_private::containers::ContainerError("No value !!");
    }
    return std::move(value_);
  }
  CONSTEXPR void reset() noexcept {
    is_present_ = false;
    value_ = {};
  }

 private:
  bool is_present_ = false;
  T value_;
};

}

}

}

#endif //ICC_OPTIONAL_HPP
