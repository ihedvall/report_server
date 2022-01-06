/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <array>
#include <vector>
#include <boost/asio.hpp>
#include "util/threadsafequeue.h"
#include "listenmessage.h"

namespace util::log::detail {
class ListenServer;

class ListenServerConnection final {
 public:
  ListenServerConnection(ListenServer& server, std::unique_ptr<boost::asio::ip::tcp::socket>& socket);
  ~ListenServerConnection();

  ListenServerConnection() = delete;
  ListenServerConnection(ListenServerConnection& ) = delete;
  ListenServerConnection& operator = (ListenServerConnection&) = delete;

  bool Cleanup();
  void InMessage(std::unique_ptr<ListenMessage> msg);
 private:
  ListenServer& server_;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
  std::array<uint8_t, 8> header_data_ {0};
  std::vector<uint8_t> body_data_;
  ThreadSafeQueue<ListenMessage> msg_queue_;
  std::vector<uint8_t> msg_data_;
  boost::asio::steady_timer queue_timer_;

  void DoReadHeader();
  void DoReadBody();
  void DoMessageQueue();
  void DoQueueTimer();
  void Close();
  void HandleMessage();
};
} // end namespace