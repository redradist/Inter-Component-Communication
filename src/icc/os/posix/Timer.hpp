//
// Created by redra on 01.06.19.
//

#ifndef ICC_OS_TIMER_HPP
#define ICC_OS_TIMER_HPP

namespace icc {

namespace os {

namespace posix {

class Timer {
 public:
  static Timer createTimer();

  void start(unsigned microSec);
  void onTimerExpired(int fd);

 private:
  friend class EventLoop;

  explicit Timer(int timerFd);

  const int timer_fd_;
};

}

}

}

#endif //ICC_OS_TIMER_HPP
