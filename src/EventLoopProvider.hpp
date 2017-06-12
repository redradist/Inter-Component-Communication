/**
 * @file EventLoopProvider.hpp
 * @author Denis Kotov
 * @date 13 Jun 2017
 * @brief Contains abstract class for Event Loop Provider
 * @copyright MIT License. Open source:
 */

#ifndef ICC_EVENTLOOPPROVIDER_HPP
#define ICC_EVENTLOOPPROVIDER_HPP

#include <memory>
#include <boost/asio/io_service.hpp>

class IEventLoopProvider {
 public:
  virtual std::shared_ptr<boost::asio::io_service> getEventLoop() const = 0;
};

#endif //ICC_EVENTLOOPPROVIDER_HPP
