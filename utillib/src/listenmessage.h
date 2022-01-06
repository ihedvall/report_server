/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <atomic>
#include <map>
#include "util/timestamp.h"
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
namespace util::log::detail {

struct SharedListenMessage {
  uint64_t ns1970 = time::TimeStampToNs();
  char pre_text[10] {};
  char text[100] {};
};

struct SharedListenQueue {
  boost::interprocess::interprocess_recursive_mutex locker;      ///< Mutex that locks the queue
  boost::interprocess::interprocess_semaphore message_semaphore; ///< Semaphore used by master waiting for messages

  std::atomic<bool> active = false;       ///< If false, no messages should be handled.
  std::atomic<uint8_t> log_level = 0;     ///< Store the current log level.
  std::atomic<uint16_t> nof_messages = 0; ///< Number of messages in the queue.
  uint8_t queue_in = 0;                   ///< In-coming message index.
  uint8_t queue_out = 0;                  ///< Out-going message index.
  SharedListenMessage queue[256] {};      ///< Message queue

  SharedListenQueue();;
};

enum class ListenMessageType : uint16_t {
  LogLevelText = 0,
  TextMessage,
  LogLevel
};

class ListenMessage {
 public:
  ListenMessageType type_ = ListenMessageType::LogLevel;
  uint16_t          version_ = 0;
  uint32_t          body_size_ = 0;
  virtual void ToBuffer(std::vector<uint8_t>& dest);
  void FromHeaderBuffer(const std::array<uint8_t,8>& source);
};

class LogLevelTextMessage : public ListenMessage {
 public:
  std::map<uint64_t, std::string> log_level_text_list_;
  LogLevelTextMessage();
  void ToBuffer(std::vector<uint8_t> &dest) override;
  void FromBodyBuffer(const std::vector<uint8_t> &source);
};

class LogLevelMessage : public ListenMessage {
 public:
  uint64_t log_level_ = 0;
  LogLevelMessage();
  void ToBuffer(std::vector<uint8_t> &dest) override;
  void FromBodyBuffer(const std::vector<uint8_t> &source);
};
class ListenTextMessage : public ListenMessage {
 public:
  uint64_t ns1970_ = 0;
  std::string pre_text_;
  std::string text_;
  ListenTextMessage();
  explicit ListenTextMessage(const SharedListenMessage& msg);
  void ToBuffer(std::vector<uint8_t> &dest) override;
  void FromBodyBuffer(const std::vector<uint8_t> &source);
};

} // end namespace util::log::detail
