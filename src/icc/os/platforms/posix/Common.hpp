//
// Created by redra on 07.06.19.
//

#ifndef POSIX_COMMON_HPP
#define POSIX_COMMON_HPP

namespace icc {
namespace os {

struct Handle {
  int fd_;
};

constexpr Handle kInvalidHandle{-1};

enum class EventType {
  READ,
  WRITE,
  ERROR,
};

}
}

#endif //POSIX_COMMON_HPP
