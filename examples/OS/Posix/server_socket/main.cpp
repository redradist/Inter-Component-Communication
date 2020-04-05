#include <iostream>
#include <thread>
#include <icc/os/networking/ServerSocket.hpp>
#include <functional>

class ClientSocket : public icc::os::IServerSocketListener {
 public:
  void onNewClientSocket(std::shared_ptr<icc::os::Socket> _client) override {
    std::cout << "New client connected ..." << std::endl;
    auto rcvData = _client->receive();
    std::string nsdasd{"Hid Denis .."};
    _client->send({nsdasd.begin(), nsdasd.end()});
    for (uint8_t ch : rcvData) {
      std::cout << "Data ch: " << ch << std::endl;
    }
  }
};

int main() {
  auto socket = icc::os::ServerSocket::createServerSocket("127.0.0.1", 50000, 10);
  ClientSocket cl;
  socket->addListener(&cl);
  std::this_thread::sleep_for(std::chrono::seconds(25));
  return 0;
}
