/**
 * @file ProcessBus.cpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains ProcessBus class.
 * It is a broker pattern. Used to control registration
 * and connection of services
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#include "ProcessBus.hpp"

namespace icc {

namespace localbus {

ProcessBus::ProcessBus()
  : IComponent(nullptr) {
  thread_ = std::thread([=]() {
    exec();
  });
}

ProcessBus::~ProcessBus() {
  exit();
  thread_.join();
}

ProcessBus &
ProcessBus::getBus() {
  static ProcessBus bus;
  return bus;
}

}

}