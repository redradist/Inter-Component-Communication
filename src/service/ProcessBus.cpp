/**
 * @file ProcessBus.cpp
 * @author Denis Kotov
 * @date 25 Jun 2017
 * @brief Contains ProcessBus class.
 * @copyright Denis Kotov, MIT License. Open source:
 */

#include "ProcessBus.hpp"

ProcessBus::ProcessBus() {
  thread_ = std::thread([=]() {
    exec();
  });
}

ProcessBus::~ProcessBus() {
  thread_.join();
}

ProcessBus &
ProcessBus::getBus() {
  static ProcessBus bus;
  return bus;
}
