//
// Created by redra on 01.06.19.
//

#ifndef ICC_OSEVENTLOOP_HPP
#define ICC_OSEVENTLOOP_HPP

#include <icc/EventLoop.hpp>

namespace icc {

namespace os {

class EventLoop : public IEventLoop {

};

}

}

//int main() {
//  auto timer = Timer::createTimer();
//  timer.start(10000000);
//  while (1) {
//  }
//  return 0;
//}




//#include <iostream>
//#include <errno.h>
//#include <inttypes.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <string.h>
//#include <sys/epoll.h>
//#include <sys/timerfd.h>
//#include <vector>
//#include <functional>
//
//struct periodic_info
//{
//  int timer_fd;
//  unsigned long long wakeups_missed;
//};
//
//itimerspec get_timespec(unsigned int microsec);
//int make_periodic(int timerfd, itimerspec period, periodic_info *info);
//
//int main() {
//  const int timerfd = timerfd_create (CLOCK_MONOTONIC, 0);
//  struct itimerspec itval = get_timespec(10000000);
//  struct periodic_info info;
//  make_periodic(timerfd, itval, &info);
//
//  int count = 0;
//  while (true) {
//    fd_set readfds;
//    FD_ZERO(&readfds);
//    FD_SET(info.timer_fd, &readfds);
//
//    select(info.timer_fd+1, &readfds, nullptr, nullptr, nullptr);
//    timerfd_gettime(timerfd, &itval);
//    uint64_t numberExpired;
//    read(timerfd, &numberExpired, sizeof(numberExpired));
//    printf("read() returned res=%" PRIu64 "\n", numberExpired);
//    if (itval.it_value.tv_sec == 0 && itval.it_value.tv_nsec == 0) {
//      std::cout << "Timer expired !!" << itval.it_value.tv_sec << std::endl;
//    }
//    make_periodic(timerfd, itval, &info);
//  }
//
//  return 0;
//}
//
//itimerspec get_timespec(unsigned int microsec) {
//  struct itimerspec itval;
//  unsigned int ns;
//  unsigned int sec;
//  /* Make the timer periodic */
//  sec = microsec / 1000000;
//  ns = 0;
//  itval.it_interval.tv_sec = sec;
//  itval.it_interval.tv_nsec = ns;
//  itval.it_value.tv_sec = sec;
//  itval.it_value.tv_nsec = ns;
//  return itval;
//}
//
//int make_periodic(int timerfd, struct itimerspec itval, periodic_info *info)
//{
//  if (timerfd == -1)
//    return timerfd;
//
//  int ret;
//  /* Create the timer */
//  info->timer_fd = timerfd;
//  info->wakeups_missed = 0;
//  ret = timerfd_settime (timerfd, 0, &itval, NULL);
//  return ret;
//}

#endif //ICC_OSEVENTLOOP_HPP
