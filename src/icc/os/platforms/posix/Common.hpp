//
// Created by redra on 07.06.19.
//

#ifndef POSIX_COMMON_HPP
#define POSIX_COMMON_HPP

namespace icc {
namespace os {

struct Handle {
  int fd_;

  bool operator==(const Handle & other) const {
    return fd_ == other.fd_;
  }

  bool operator!=(const Handle & other) const {
    return !operator==(other);
  }
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
