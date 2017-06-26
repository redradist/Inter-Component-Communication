//
// Created by redra on 25.06.17.
//

#include "ProcessBus.hpp"

ProcessBus &
ProcessBus::getBus() {
  static ProcessBus bus;
  return bus;
}
