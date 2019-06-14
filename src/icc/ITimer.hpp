//
// Created by redra on 05.06.19.
//

#ifndef POSIX_ITIMER_HPP
#define POSIX_ITIMER_HPP

namespace icc {

class ITimer {
 public:
  /**
   * Enable continuous mode
   */
  virtual void enableContinuous() = 0;

  /**
   * Disable continuous mode
   */
  virtual void disableContinuous() = 0;

  /**
   * Setting number of repetitions
   * @param number Number of repetition
   */
  virtual void setNumberOfRepetition(const int32_t &number) = 0;

  /**
   * Setting interval mode for the timer
   * @param _duration Timeout duration in boost::posix_time::time_duration
   */
  virtual void setInterval(const std::chrono::nanoseconds _duration) = 0;

  /**
   * Method is used to start async waiting timer
   */
  virtual bool start() = 0;

  /**
   * Method is used to stop waiting timer
   */
  virtual bool stop() = 0;
};

}

#endif //POSIX_ITIMER_HPP
