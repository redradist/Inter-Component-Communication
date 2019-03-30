/**
 * @file State.hpp
 * @author Denis Kotov
 * @date 24 Mar 2018
 * @brief Contains Command State
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef COMMONAPI_CLIENT_COMMANDSTATE_HPP
#define COMMONAPI_CLIENT_COMMANDSTATE_HPP

namespace icc {

namespace command {

enum class State {
  INACTIVE,
  ACTIVE,
  SUSPENDED,
  FINISHED,
};

}

}

#endif //COMMONAPI_CLIENT_COMMANDSTATE_HPP
