#include <iostream>
#include <vector>
#include <future>
#include <experimental/coroutine>
#include <chrono>

#include <coroutine/Task.hpp>
#include <coroutine/Timer.hpp>
#include <coroutine/TaskScheduler.hpp>

std::promise<int> promise;
std::future<int> compute_value2()
{
  return promise.get_future();
}

std::promise<float> promise2;
std::future<float> compute_value242()
{
  return promise2.get_future();
}

icc::coroutine::Task<float> compute_value3()
{
  auto result = co_await compute_value242();

  std::cout << "icc::Task<float> compute_value3(): result = " << result << std::endl;
  co_return result;
}

icc::coroutine::Task<void> compute_value4()
{
  co_return;
}

icc::coroutine::Task<void> compute_value()
{
  auto value0 = co_await compute_value2();
  auto value1 = co_await compute_value3();
  std::cout << "Start timer" << std::endl;
  co_await std::chrono::duration< long, std::ratio<60> >(1);
  std::cout << "Finished timer" << std::endl;
//  co_await std::chrono::hours(1);
//  co_await std::chrono::minutes(1);
//  co_await std::chrono::seconds(1);
//  co_await std::chrono::milliseconds(1);
//  co_await std::chrono::microseconds(1);
//  co_await boost::posix_time::time_duration();
//  co_await boost::posix_time::hours(5);
  std::cout << "Start timer" << std::endl;
  co_await boost::posix_time::minutes(1);
//  co_await boost::posix_time::seconds(5);
//  co_await boost::posix_time::milliseconds(5);
//  co_await boost::posix_time::microseconds(5);
  std::cout << "Finished timer" << std::endl;
  co_await compute_value4();
  std::cout << "Task is finihed" << std::endl;
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
  service->run();
  worker0.join();
  worker1.join();
  return 0;
}
