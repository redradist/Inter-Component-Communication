#include <iostream>
#include <thread>
#include <CommonAPI/CommonAPI.hpp>
#include "HelloWorldStubImpl.hpp"

using namespace std;

int main() {
  auto helloWorldService = std::make_shared<HelloWorldStubImpl>();
  while (true) {
    helloWorldService->registerService("local", "test");
    std::cout << "Waiting for calls... (Abort with CTRL+C)" << std::endl;
    helloWorldService->exec();
    std::this_thread::sleep_for(std::chrono::seconds(30));
    helloWorldService->unregisterService();
  }
  return 0;
}
