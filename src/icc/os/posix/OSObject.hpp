//
// Created by redra on 07.06.19.
//

#ifndef POSIX_OSOBJECT_HPP
#define POSIX_OSOBJECT_HPP

namespace icc {

namespace os {

struct EventLoop::OSObject {
  int fd_;
};

}

}

#endif //POSIX_OSOBJECT_HPP
