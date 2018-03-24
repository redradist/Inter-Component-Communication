//
// Created by redra on 24.03.18.
//

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
