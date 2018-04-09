//
// Created by redra on 09.04.18.
//

#include <iostream>
#include <thread>
#include <threadpool/Task.hpp>
#include <threadpool/ThreadPool.hpp>
#include <iostream>
#include <chrono>
#include <thread>

class MyComponent : public icc::IComponent {
 public:
  MyComponent ()
    : icc::IComponent(nullptr) {

  }
  void newFunction(int _result) {
    std::cout << "newFunction(int result = " << _result << ")" << std::endl;
    exit();
  }
};

int main() {
  MyComponent comp;
  icc::pools::Task<int>([=] () -> int {
    std::cout << "icc::pools::Task<int>" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 1;
  }).then([=] (int result1) {
    std::cout << "then([=] (int result = " << result1 << "))" << std::endl;
  }).callback(&MyComponent::newFunction, &comp)
  .start();
  icc::pools::Task<int>([=] () -> int {
    std::cout << "icc::pools::Task<int>" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 2;
  }).then([=] (int result2) {
    std::cout << "then([=] (int result = " << result2 << "))" << std::endl;
  }).start();
  icc::pools::Task<int>([=] () -> int {
    std::cout << "icc::pools::Task<int>" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 5;
  }).then([=] (int result) {
    std::cout << "then([=] (int result = " << result << "))" << std::endl;
  }).start();
//  icc::pools::ThreadPool::getPool().push(task);
  std::this_thread::sleep_for(std::chrono::seconds(8));
  comp.exec();
  return 0;
}
