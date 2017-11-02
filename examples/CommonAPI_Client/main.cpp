#include <iostream>
#include <thread>
#include <CommonAPI/CommonAPI.hpp>
#include "HelloWorldProxyClient.hpp"

using namespace std;

int main() {
  HelloWorldProxyClient helloWorldClient("local", "test");
  helloWorldClient.getProxyStatusEvent().subscribe(
  [&helloWorldClient](const CommonAPI::AvailabilityStatus & _status) mutable {
    if (CommonAPI::AvailabilityStatus::AVAILABLE == _status) {
      helloWorldClient.sayHelloAsync("Denis");
    }
  });
  while (true) {
    std::cout << "Waiting for calls... (Abort with CTRL+C)" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));
  }
  return 0;
}
