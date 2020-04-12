#include <iostream>
#include <icc/_private/containers/Optional.hpp>

#if __cpp_lib_optional >= 201606L
#include <optional>
#endif

template <typename T>
using Optional = icc::_private::containers::Optional<T>;

int main() {
  std::cout << "Hello, World!" << std::endl;
  Optional<int> val;
  std::cout << "val.has_value: " << val.has_value() << std::endl;
  std::cout << "static_cast<bool>(val): " << static_cast<bool>(val) << std::endl;
//  std::cout << "val.value: " << val.value() << std::endl;
  val = 3;
  std::cout << "val.has_value: " << val.has_value() << std::endl;
  std::cout << "static_cast<bool>(val): " << static_cast<bool>(val) << std::endl;
  std::cout << "val.value: " << val.value() << std::endl;
  val.reset();
  std::cout << "val.has_value: " << val.has_value() << std::endl;
  std::optional<int> std_val;
  std_val = val;
  return 0;
}
