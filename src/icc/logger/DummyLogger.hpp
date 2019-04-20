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
  template <typename ... TArgs>
  inline void debug(const std::string & _str, TArgs && ... args) {
    // Dummy implementation
  }

  template <typename ... TArgs>
  inline void info(const std::string & _str, TArgs && ... args) {
    // Dummy implementation
  }

  template <typename ... TArgs>
  inline void warning(const std::string & _str, TArgs && ... args) {
    // Dummy implementation
  }

  template <typename ... TArgs>
  inline void error(const std::string & _str, TArgs && ... args) {
    // Dummy implementation
  }
};

}

}

#endif //ICC_DUMMYLOGGER_HPP
