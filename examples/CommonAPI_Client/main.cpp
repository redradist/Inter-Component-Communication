#include <iostream>
#include <thread>
#include <CommonAPI/CommonAPI.hpp>
#include "HelloWorldProxyClient.hpp"

using namespace std;

int main() {
  auto helloWorldClient = std::make_shared<HelloWorldProxyClient>("local", "test");
//  helloWorldClient.getProxyStatusEvent().subscribe(
//  [&helloWorldClient](const CommonAPI::AvailabilityStatus & _status) mutable {
//    if (CommonAPI::AvailabilityStatus::AVAILABLE == _status) {
//      helloWorldClient.requestSayHello("Denis");
//    }
//  });
  while (true) {
    std::cout << "Waiting for calls... (Abort with CTRL+C)" << std::endl;
    helloWorldClient->exec();
    std::this_thread::sleep_for(std::chrono::seconds(30));
  }
  return 0;
}
