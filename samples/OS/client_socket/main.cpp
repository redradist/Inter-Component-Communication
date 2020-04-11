#include <iostream>
#include <thread>
#include <icc/os/networking/Socket.hpp>
#include <functional>

int main() {
  auto socket = icc::os::Socket::createSocket("127.0.0.1", 50000);

  std::string nsdasd{"Hello Denis !!"};
  socket->send({nsdasd.begin(), nsdasd.end()});
  auto rcvData = socket->receive();
  std::cout << "Data: " << std::string(rcvData.begin(), rcvData.end()) << std::endl;
  return 0;
}
