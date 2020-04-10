#include <iostream>

#include <icc/os/networking/Socket.hpp>

int main() {
  std::cout << "Hello, World!" << std::endl;
  auto socket = icc::os::Socket::createSocket("", 80);
  socket->send();
  auto status = socket->sendAsync();
  status.wait();

  auto chunk = socket->receive();
  auto chunk2 = socket->receiveAsync();
  auto chunk3 = socket->receiveAsync();
  return 0;
}
