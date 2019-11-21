//
// Created by redra on 18.07.19.
//

#ifndef ICC_SOCKETIMPL_HPP
#define ICC_SOCKETIMPL_HPP

#include <memory>
#include <chrono>
#include <vector>
#include <future>

#include <icc/ITimerListener.hpp>
#include <icc/os/EventLoop.hpp>
#include <icc/os/networking/Socket.hpp>

#include "Common.hpp"

namespace icc {

namespace os {

struct Handle;

using ChunkData = std::vector<uint8_t>;

struct SendChunk {
  ChunkData data_;
  std::unique_ptr<std::promise<void>> promise_ptr_;
};

class Socket::SocketImpl : public ISocket {
 public:
  ~SocketImpl() = default;

  void send(ChunkData _data) override;
  std::future<void> sendAsync(ChunkData _data, ISocketSender &_sender) override;
  ChunkData receive() override;
  std::future<ChunkData> receiveAsync(ISocketReceiver & _receiver) override;

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(std::shared_ptr<ISocketListener> _listener) override;

  /**
   * Method is used to add the listener
   * @param _listener Listener that is being adding
   */
  void addListener(ISocketListener * _listener) override;

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(std::shared_ptr<ISocketListener> _listener) override;

  /**
   * Method is used to remove the listener
   * @param _listener Listener that is being removing
   */
  void removeListener(ISocketListener * _listener) override;

 private:
  friend class EventLoop;

  explicit SocketImpl(const Handle & socketHandle);
  void onSocketDataAvailable(const Handle &_);
  void onSocketBufferAvailable(const Handle &_);

  Handle socket_handle_{kInvalidHandle};
  std::vector<SendChunk> buffer_chunks_;
  std::atomic<size_t> current_sent_data_{0};
  std::promise<bool> async_buffer_status_;
  std::atomic_bool buffer_available_event_{false};
  std::mutex mtx_;
  std::condition_variable cond_var_;
};

}

}

#endif //ICC_SOCKETIMPL_HPP
