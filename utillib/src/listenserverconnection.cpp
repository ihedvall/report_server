/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <memory>
#include <boost/asio.hpp>
#include "util/logstream.h"
#include "listenmessage.h"
#include "listenserverconnection.h"
#include "listenserver.h"

using namespace util::log;
using namespace boost::asio;
using namespace std::chrono_literals;

namespace util::log::detail {

ListenServerConnection::ListenServerConnection(ListenServer& server,
                                               std::unique_ptr<boost::asio::ip::tcp::socket> &socket)
    : server_(server),
      socket_(std::move(socket)),
      queue_timer_(server.Context()) {
  DoReadHeader();

  auto msg1 = std::make_unique<LogLevelTextMessage>();
  msg1->log_level_text_list_ = server.LogLevelList();
  InMessage(std::move(msg1));

  auto msg2 = std::make_unique<LogLevelMessage>();
  msg2->log_level_ = server.LogLevel();
  InMessage(std::move(msg2));

  DoMessageQueue();
}

ListenServerConnection::~ListenServerConnection() {
  if (socket_ && socket_->is_open()) {
    socket_->close();
  }
  socket_.reset();
}

bool ListenServerConnection::Cleanup() {
  return !socket_ || !socket_->is_open();
}

void ListenServerConnection::DoReadHeader() { // NOLINT
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  async_read(*socket_, boost::asio::buffer(header_data_),
             [&] (const boost::system::error_code& error, size_t bytes) { // NOLINT
               if (error && error == error::eof) {
                 LOG_INFO() << "Connection closed by remote";
                 Close();
               } else if (error) {
                 LOG_ERROR() << "Listen header error. Error: " << error.message();
                 Close();
               } else if (bytes != header_data_.size() ) {
                 LOG_ERROR() << "Listen header length error. Error: " << error.message();
                 Close();
               } else {
                 ListenMessage header;
                 header.FromHeaderBuffer(header_data_);
                 if (header.body_size_ > 0) {
                   body_data_.clear();
                   body_data_.resize(header.body_size_,0);
                   DoReadBody();
                 } else {
                   LOG_ERROR() << "Listen header error. Error: " << error.message();
                   Close();
                 }
               }
             });
}

void ListenServerConnection::DoReadBody() { // NOLINT
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  async_read(*socket_, boost::asio::buffer(body_data_),
             [&] (const boost::system::error_code& error, size_t bytes) { // NOLINT
               if (error) {
                 LOG_ERROR() << "Listen body error. Error: " << error.message();
                 Close();
               } else if (bytes != body_data_.size() ) {
                 LOG_ERROR() << "Listen body length error. Error: " << error.message();
                 Close();
               } else {
                 HandleMessage();
                 DoReadHeader();
               }
             });
}

void ListenServerConnection::Close() {
  boost::system::error_code dummy;
  socket_->shutdown(ip::tcp::socket::shutdown_both, dummy);
  socket_->close(dummy);
}

void ListenServerConnection::HandleMessage() {
  ListenMessage header;
  header.FromHeaderBuffer(header_data_);
  switch (header.type_) {
    case ListenMessageType::LogLevelText: {
      auto msg = std::make_unique<LogLevelTextMessage>();
      msg->FromHeaderBuffer(header_data_);
      msg->FromBodyBuffer(body_data_);
      server_.InMessage(std::move(msg));
      break;
    }

    case ListenMessageType::LogLevel: {
      auto msg = std::make_unique<LogLevelMessage>();
      msg->FromHeaderBuffer(header_data_);
      msg->FromBodyBuffer(body_data_);
      server_.InMessage(std::move(msg));
      break;
    }

    case ListenMessageType::TextMessage:
      LOG_ERROR() << "Illegal text message received.";
      break;

    default: {
      LOG_ERROR() << "Unknown message type. Type: " << static_cast<int>(header.type_);
      break;
    }
  }
}

void ListenServerConnection::InMessage(std::unique_ptr<ListenMessage> msg) {
  msg_queue_.Put(msg);
}

void ListenServerConnection::DoMessageQueue() { // NOLINT
  if (!socket_ || !socket_->is_open()) {
    return;
  }
  std::unique_ptr<ListenMessage> msg;
  bool message = msg_queue_.Get(msg, false);
  if (message) {
    msg_data_.clear();
    msg->ToBuffer(msg_data_);
    async_write(*socket_, boost::asio::buffer(msg_data_),
        [&](const boost::system::error_code &error, size_t bytes) { // NOLINT
          if (error) {
            LOG_ERROR() << "Listen body error. Error: " << error.message();
            Close();
          } else if (bytes != msg_data_.size()) {
            LOG_ERROR() << "Listen message length error. Error: " << error.message();
            Close();
          } else {
            DoMessageQueue();
          }
        });
  } else {
    DoQueueTimer();
  }
}

void ListenServerConnection::DoQueueTimer() {
  queue_timer_.expires_after(20ms);
  queue_timer_.async_wait([&] (const boost::system::error_code error) {
    if (error) {
      LOG_ERROR() << "Queue timer error. Error: " << error.message();
    } else {
      DoMessageQueue();
    }
  });

}

}

