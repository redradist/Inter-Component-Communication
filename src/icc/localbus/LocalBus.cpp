/**
 * @file LocalBus.cpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains LocalBus class.
 * It is a broker pattern. Used to control registration
 * and connection of services
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include "LocalBus.hpp"

namespace icc {

namespace localbus {

LocalBus::LocalBus()
  : IComponent(nullptr) {
  thread_ = std::thread([=]() {
    exec();
  });
}

LocalBus::~LocalBus() {
  exit();
  thread_.join();
}

LocalBus &
LocalBus::getBus() {
  static LocalBus bus;
  return bus;
}

}

}
