/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <string>
#include <boost/asio.hpp>
#include "util/ilisten.h"
#include "util/threadsafequeue.h"
#include "listenmessage.h"
#include "listenserverconnection.h"
#include "messagequeue.h"

namespace util::log::detail {

class ListenServer : public IListen {
 public:
  ListenServer();
  explicit ListenServer(const std::string& share_name);
  ~ListenServer() override;
  boost::asio::io_context& Context();
  [[nodiscard]] bool IsActive() const override;
  bool Start() override;
  bool Stop() override;

  void InMessage(std::unique_ptr<ListenMessage> msg);

  void ShareName(const std::string& share_name);
  [[nodiscard]] std::string ShareName() const;

  [[nodiscard]] size_t LogLevel() override;
 protected:
  void AddMessage(uint64_t nano_sec_1970, const std::string &pre_text, const std::string &text) override;
 private:
  boost::asio::io_context context_;
  boost::asio::steady_timer cleanup_timer_;
  std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
  std::unique_ptr<boost::asio::ip::tcp::socket> connection_socket_;
  std::thread worker_thread_;

  std::mutex connection_list_lock_;
  std::deque<std::unique_ptr<ListenServerConnection>> connection_list_;

  ThreadSafeQueue<ListenMessage> msg_queue_;
  boost::asio::steady_timer queue_timer_;

  std::atomic<uint64_t> log_level_ = 0;
  std::atomic<bool> active_ = false;

  std::unique_ptr<MessageQueue> share_mem_queue_;

  void WorkerTask();
  void DoAccept();
  void DoCleanup();
  void DoMessageQueue();
  void HandleMessage(const ListenMessage* msg);
};

} // end namespace




