//
// Created by RedRa on 15.04.2020.
//

#ifndef ICC_COMMON_HPP
#define ICC_COMMON_HPP

#include <windows.h>

namespace icc {
namespace os {

struct Handle {
    HANDLE handle_;
};

constexpr Handle kInvalidHandle{nullptr};

enum class EventType {
  kRead,
  kWrite,
  kError,
};

}
}

#endif //ICC_COMMON_HPP
