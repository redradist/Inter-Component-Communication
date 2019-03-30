/**
 * @file ICommandData.hpp
 * @author Denis Kotov
 * @date 31 Mar 2018
 * @brief
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMANDLOOP_ICOMMANDDATA_HPP
#define COMMANDLOOP_ICOMMANDDATA_HPP

#include "State.hpp"

namespace icc {

namespace command {

class ICommandData {
 public:
  virtual State getState() const = 0;
  virtual int getCommandType() const = 0;
};

}

}

#endif //COMMANDLOOP_ICOMMANDDATA_HPP
