#include <iostream>
#include <thread>
#include <icc/os/networking/ServerSocket.hpp>
#include <functional>

int main() {
  auto socket = icc::os::ServerSocket::createServerSocket("127.0.0.1", 50000, 10);
  auto clientSocket = socket->accept();
  std::cout << "New client connected ..." << std::endl;
  auto rcvData = clientSocket->receive();
  std::cout << "Data: " << std::string(rcvData.begin(), rcvData.end()) << std::endl;
  std::string response{"Hi, Denis .."};
  clientSocket->send({response.begin(), response.end()});
  std::this_thread::sleep_for(std::chrono::seconds(5));
  return 0;
}
