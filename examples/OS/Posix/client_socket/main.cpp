#include <iostream>
#include <thread>
#include <icc/os/networking/Socket.hpp>
#include <functional>

int main() {
  auto socket = icc::os::Socket::createSocket("127.0.0.1", 50000);

  std::string nsdasd{"Hello Denis !!"};
  socket->send({nsdasd.begin(), nsdasd.end()});
  auto rcvData = socket->receive();
  std::this_thread::sleep_for(std::chrono::seconds(25));
  for (uint8_t ch : rcvData) {
    std::cout << "Data ch: " << ch << std::endl;
  }
  return 0;
}
