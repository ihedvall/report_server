/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include "listenmessage.h"

namespace util::log::detail {

class MessageQueue {
 public:
  MessageQueue(bool master, const std::string& shared_mem_name);
  virtual ~MessageQueue();

  MessageQueue() = delete;
  MessageQueue(const MessageQueue& ) = delete;
  MessageQueue& operator = (const MessageQueue&) = delete;

  [[nodiscard]] bool IsActive() const {
    return active_;
  }

  [[nodiscard]] uint8_t LogLevel() const {
    return log_level_;
  }

  void SetActive(bool active);
  void SetLogLevel(uint8_t log_level);

  void Add(const SharedListenMessage& msg);
  bool Get(SharedListenMessage& msg, bool block);
  [[nodiscard]] size_t NofMessages() const;
  void Stop();
 private:
  std::unique_ptr<boost::interprocess::shared_memory_object> shared_mem_;
  std::unique_ptr<boost::interprocess::mapped_region> region_;
  SharedListenQueue* queue_ = nullptr;
  bool master_ = false;
  std::string name_;

  std::thread task_;
  std::atomic<bool> task_stop_ = false;
  std::condition_variable task_event_;
  std::mutex task_mutex_;

  std::atomic<bool> active_ = false;
  std::atomic<uint8_t> log_level_ = 0;

  void ClientTask();

};

} // end namespace util::detail


