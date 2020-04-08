#include <iostream>
#include <thread>
#include <icc/os/networking/ServerSocket.hpp>
#include <functional>

class ClientSocket : public icc::os::IServerSocketListener {
 public:
  void onNewClientSocket(std::shared_ptr<icc::os::Socket> _client) override {
    std::thread([_client] {
      std::cout << "New client connected ..." << std::endl;
      auto rcvData = _client->receive();
      std::cout << "Data: " << std::string(rcvData.begin(), rcvData.end()) << std::endl;
      std::string response{"Hi, Denis .."};
      _client->send({response.begin(), response.end()});
    }).detach();
  }
};

int main() {
  auto socket = icc::os::ServerSocket::createServerSocket("127.0.0.1", 50000, 10);
  ClientSocket cl;
  socket->addListener(&cl);
  std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}
