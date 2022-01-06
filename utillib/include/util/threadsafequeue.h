/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <mutex>
#include <queue>
#include <memory>

namespace util::log {

template <typename T>
class ThreadSafeQueue
{
 public:
  ThreadSafeQueue() = default;
  virtual ~ThreadSafeQueue();

  ThreadSafeQueue(const ThreadSafeQueue&) = delete;
  ThreadSafeQueue& operator = (const ThreadSafeQueue&) = delete;

  void Put(std::unique_ptr<T>& value);
  [[nodiscard]] bool Get(std::unique_ptr<T>& dest, bool block);
  [[nodiscard]] bool Empty() const;
  [[nodiscard]] size_t Size() const;
 private:
  mutable std::mutex lock_;
  std::queue<std::unique_ptr<T>> queue_;
  std::atomic<bool> stop_ = false;
  std::condition_variable queue_event_;
};

template<typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() {
  stop_ = true;
  queue_event_.notify_one(); // Release any blocking Get()
  std::lock_guard lock(lock_); // Waiting for Get() to release
  while (!queue_.empty() ) {
   queue_.pop();
  }
}

template<typename T>
void ThreadSafeQueue<T>::Put(std::unique_ptr<T>& value) {
  if (stop_) {
    return;
  }
  std::lock_guard lock(lock_);
  queue_.push(std::move(value));
  queue_event_.notify_one();
}

template<typename T>
bool ThreadSafeQueue<T>::Get(std::unique_ptr<T> &dest, bool block) {
  if (stop_) {
    return false;
  }

  if (block) {
    std::unique_lock lock(lock_);
    queue_event_.wait(lock,[&] {
      return !queue_.empty() || stop_.load();
    });
    if (queue_.empty() || stop_) {
      return false;
    }
    dest = std::move(queue_.front());
    queue_.pop();
  } else {
    std::lock_guard lock(lock_);
    if (queue_.empty() || stop_) {
      return false;
    }
    dest = std::move(queue_.front());
    queue_.pop();
  }
  return true;
}

template<typename T>
bool ThreadSafeQueue<T>::Empty() const {
  std::lock_guard lock(lock_);
  return queue_.empty();
}

template<typename T>
size_t ThreadSafeQueue<T>::Size() const {
  std::lock_guard lock(lock_);
  return queue_.size();
}

}