/**
 * @file DummyLogger.hpp
 * @author Denis Kotov
 * @date 17 Jul 2017
 * @brief Contains DummyLogger for using in templates.
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_DUMMYLOGGER_HPP
#define ICC_DUMMYLOGGER_HPP

#include <string>

namespace icc {

namespace logger {

class DummyLogger {
 public:
  inline void debug(const std::string & _str) {
    // Dummy implementation
  }

  inline void info(const std::string & _str) {
    // Dummy implementation
  }

  inline void warning(const std::string & _str) {
    // Dummy implementation
  }

  inline void error(const std::string & _str) {
    // Dummy implementation
  }
};

}

}

#endif //ICC_DUMMYLOGGER_HPP
