#include <iostream>
#include <vector>
#include <future>
#include <experimental/coroutine>
#include <chrono>

#include <icc/coroutine/Task.hpp>
#include <icc/coroutine/Timer.hpp>
#include <icc/coroutine/TaskScheduler.hpp>

std::promise<int> promise;
std::future<int> compute_value2() {
  return promise.get_future();
}

std::promise<float> promise2;
std::future<float> compute_value242() {
  return promise2.get_future();
}

icc::coroutine::Task<float> compute_value3() {
  auto result = co_await compute_value242();

  std::cout << "icc::Task<float> compute_value3(): result = " << result << std::endl;
  co_return result;
}

icc::coroutine::Task<void> compute_value4() {
  co_return;
}

icc::coroutine::Task<void> compute_value() {
  auto value0 = co_await compute_value2();
  auto value1 = co_await compute_value3();
  std::cout << "Start timer in compute_value" << std::endl;
  co_await std::chrono::seconds(1);
  std::cout << "Finished timer in compute_value" << std::endl;
  co_await compute_value4();
  std::cout << "Task is finihed. Computed value is "
            << (value0 + value1) << std::endl;
}

icc::coroutine::Task<void> compute_value10() {
  std::cout << "Start timer in compute_value10" << std::endl;
  co_await std::chrono::seconds(4);
  std::cout << "Finished timer in compute_value10" << std::endl;
}

icc::coroutine::Task<void> compute_value8() {
  std::cout << "Start timer in compute_value8" << std::endl;
  co_await std::chrono::seconds(2);
  std::cout << "Finished timer in compute_value8" << std::endl;
  co_await std::chrono::seconds(1);
}

int main() {
  std::thread worker0 = std::thread([=] {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    promise.set_value(1);
  });
  std::thread worker1 = std::thread([=] {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    promise2.set_value(43);
  });
  auto service = std::make_shared<boost::asio::io_service>();
  auto & sheduler = icc::coroutine::TaskScheduler::getDefaultTaskSheduler(service.get());
  sheduler.startCoroutine(compute_value());
  sheduler.startCoroutine(compute_value10());
  sheduler.startCoroutine(compute_value8());
  service->run();
  worker0.join();
  worker1.join();
  return 0;
}
