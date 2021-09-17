/**
 * @file ThreadSafeQueue.hpp
 * @author Denis Kotov
 * @date 11 May 2019
 * @brief Contains helper ThreadSafeQueue for multiple providers and multiple consumers
 * @copyright Denis Kotov, MIT License. Open source: https://github.com/redradist/Inter-Component-Communication.git
 */

#ifndef ICC_THREAD_SAFE_QUEUE_HPP
#define ICC_THREAD_SAFE_QUEUE_HPP

#include <new>
#include <atomic>
#include <type_traits>
#include <mutex>
#include <condition_variable>

#include "icc/_private/containers/exceptions/ContainerError.hpp"

namespace icc {

namespace _private {

namespace containers {

template<typename TItem>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() = default;

  ~ThreadSafeQueue() {
    TItem item;
    while (tryPop(item));
  }

  template<typename TAddItem>
  void push(TAddItem &&item) {
    if (!tryPush(std::forward<TAddItem>(item))) {
      throw std::bad_alloc();
    }
  }

  template<typename TAddItem>
  bool tryPush(TAddItem &&item) noexcept {
    auto *itemPtr = new(std::nothrow) TItem(std::forward<TAddItem>(item));
    if (itemPtr == nullptr) {
      return false;
    }
    auto *addNodePtr = new(std::nothrow) QueueNode();
    if (addNodePtr == nullptr) {
      delete itemPtr;
      return false;
    }
    addNodePtr->item_ptr_ = itemPtr;
    std::lock_guard<std::mutex> lock{mtx_};
    if (back_item_ == nullptr) {
      addNodePtr->next_item_ = back_item_;
      back_item_ = addNodePtr;
      front_item_ = addNodePtr;
    } else {
      addNodePtr->next_item_ = back_item_->next_item_;
      back_item_->next_item_ = addNodePtr;
      back_item_ = addNodePtr;
    }
    ++item_count_;
    cond_var_.notify_all();
    return true;
  }

  TItem pop() {
    static_assert(std::is_copy_assignable<TItem>::value,
                  "TItem is not copy assignable !!");
    TItem item;
    std::unique_lock<std::mutex> lock{mtx_};
    if (!tryPop(lock, item)) {
      throw ContainerError("No items !!");
    }
    return item;
  }

  bool tryPop(TItem &item) {
    static_assert(std::is_copy_assignable<TItem>::value,
                  "TItem is not copy assignable !!");
    std::unique_lock<std::mutex> lock{mtx_};
    QueueNode* nodePtr = nullptr;
    bool result = tryPop(lock, nodePtr);
    if (nodePtr) {
      item = *nodePtr->item_ptr_;
      lock.unlock();
      reclaimNode(nodePtr);
    }
    return result;
  }

  TItem waitPop() {
    static_assert(std::is_copy_assignable<TItem>::value,
                  "TItem is not copy assignable !!");
    std::unique_lock<std::mutex> lock{mtx_};
    TItem item;
    QueueNode* nodePtr = nullptr;
    while (!interrupted_.load(std::memory_order_acquire) &&
           !tryPop(lock, nodePtr)) {
      cond_var_.wait(lock, [this, &lock] {
        return interrupted_.load(std::memory_order_acquire) ||
               !empty(lock);
      });
    }
    if (nodePtr) {
      item = *nodePtr->item_ptr_;
      lock.unlock();
      reclaimNode(nodePtr);
    }
    return item;
  }

  bool isInterrupt() const {
    return interrupted_.load(std::memory_order_acquire);
  }

  void interrupt() {
    {
      std::lock_guard<std::mutex> lock{mtx_};
      interrupted_.store(true, std::memory_order_release);
    }
    cond_var_.notify_all();
  }

  void reset() {
    interrupted_.store(false, std::memory_order_release);
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock{mtx_};
    return empty(lock);
  }

  unsigned count() const {
    std::lock_guard<std::mutex> lock{mtx_};
    return count(lock);
  }

 private:
  struct QueueNode {
    TItem *item_ptr_ = nullptr;
    QueueNode *next_item_ = nullptr;
  };

  bool empty(const std::unique_lock<std::mutex> &lock) const {
    return item_count_ == 0;
  }

  unsigned count(const std::unique_lock<std::mutex> &lock) const {
    return item_count_;
  }

  bool tryPop(const std::unique_lock<std::mutex> &lock, QueueNode *& nodePtr) {
    if (front_item_ == nullptr) {
      return false;
    } else {
      QueueNode *const kPrevFrontNode = front_item_;
      TItem *const kItemPtr = front_item_->item_ptr_;
      if (kPrevFrontNode == back_item_) {
        front_item_ = nullptr;
        back_item_ = nullptr;
      } else {
        QueueNode *const kNextFrontNode = front_item_->next_item_;
        front_item_ = kNextFrontNode;
      }
      nodePtr = kPrevFrontNode;
      --item_count_;
      return true;
    }
  }

  void reclaimNode(const QueueNode *nodePtr) const {
    TItem* itemPtr = nodePtr->item_ptr_;
    delete nodePtr;
    delete itemPtr;
  }

  std::condition_variable cond_var_;
  mutable std::mutex mtx_;

  std::atomic<bool> interrupted_{false};
  unsigned item_count_ = 0;
  QueueNode *front_item_ = nullptr;
  QueueNode *back_item_ = nullptr;
};

}

}

}

#endif //ICC_THREAD_SAFE_QUEUE_HPP
