//
// Created by redra on 17.07.17.
//

#ifndef FORECAST_ILOGGER_HPP
#define FORECAST_ILOGGER_HPP

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

#endif //FORECAST_ILOGGER_HPP
