//
// Created by redra on 07.06.19.
//

#ifndef POSIX_OSOBJECT_HPP
#define POSIX_OSOBJECT_HPP

namespace icc {

namespace os {

struct OSObject {
  int fd_;
};

enum class OSObjectEventType {
  READ,
  WRITE,
  ERROR,
};

}

}

#endif //POSIX_OSOBJECT_HPP
