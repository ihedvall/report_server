/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "util/logstream.h"
#include "listenclient.h"

using namespace util::log;
using namespace boost::asio;
using namespace std::chrono_literals;

namespace util::log::detail {

ListenClient::ListenClient(std::string host_name, uint16_t port)
: host_name_(std::move(host_name)),
  port_(port),
  retry_timer_(context_) {
  DoConnect();
  worker_thread_ = std::thread(&ListenClient::WorkerTask, this);
}

ListenClient::~ListenClient() {
  if (!context_.stopped()) {
    context_.stop();
    Close();
  }
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }

}

void ListenClient::WorkerTask() {
  try {
    const auto& count = context_.run();
    LOG_INFO() << "Stopped main worker thread" ;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Context error. Error: " << error.what();
  }
}
void ListenClient::DoLookup() {
  ip::tcp::resolver lookup(context_);
  lookup.async_resolve(host_name_, std::to_string(port_),
     [&] (const boost::system::error_code& error, ip::tcp::resolver::results_type result) {
       if (error) {
         LOG_ERROR() << "Lookup error. Host: " << host_name_ << ":" << port_ << ",Error: " << error.message();
         DoRetryWait();
       } else {
         socket_ = std::make_unique<ip::tcp::socket>(context_);
         endpoints_ = std::move(result);
         DoConnect();
       }
     });
}
void ListenClient::DoRetryWait() {
  Close();
  retry_timer_.expires_after(5s);
  retry_timer_.async_wait([&] (const boost::system::error_code error) {
    if (error) {
      LOG_ERROR() << "Retry timer error. Error: " << error.message();
    }
    DoLookup();
  });

}
void ListenClient::DoConnect() {
  socket_->async_connect(*endpoints_, [&] (const boost::system::error_code error) {
    if (error) {
      LOG_ERROR() << "Connect error. Error: " << error.message();
      DoRetryWait();
    } else {
     DoReadHeader();
    }
  });
}

void ListenClient::DoReadHeader() {// NOLINT
  if (!socket_ || !socket_->is_open()) {
    DoRetryWait();
    return;
  }
  async_read(*socket_, boost::asio::buffer(header_data_), [&] (const boost::system::error_code& error, size_t bytes) { // NOLINT
    if (error && error == error::eof) {
      LOG_INFO() << "Connection closed by remote";
      DoRetryWait();
    } else if (error) {
      LOG_ERROR() << "Listen header error. Error: " << error.message();
      DoRetryWait();
    } else if (bytes != header_data_.size() ) {
      LOG_ERROR() << "Listen header length error. Error: " << error.message();
      DoRetryWait();
    } else {
      ListenMessage header;
      header.FromHeaderBuffer(header_data_);
      if (header.body_size_ > 0) {
        body_data_.clear();
        body_data_.resize(header.body_size_, 0);
        DoReadBody();
      } else {
        DoReadHeader();
      }
    }
  });
}

void ListenClient::DoReadBody() { // NOLINT
  if (!socket_ || !socket_->is_open()) {
    DoRetryWait();
    return;
  }
  async_read(*socket_, boost::asio::buffer(body_data_),
     [&] (const boost::system::error_code& error, size_t bytes) { // NOLINT
       if (error) {
         LOG_ERROR() << "Listen body error. Error: " << error.message();
         DoRetryWait();
       } else if (bytes != body_data_.size() ) {
         LOG_ERROR() << "Listen body length error. Error: " << error.message();
         DoRetryWait();
       } else {
         HandleMessage();
         DoReadHeader();
       }
     });
}

void ListenClient::HandleMessage() {

  ListenMessage header;
  header.FromHeaderBuffer(header_data_);
  std::unique_ptr<ListenMessage> message;
  switch (header.type_) {
    case ListenMessageType::LogLevelText: {
      auto msg = std::make_unique<LogLevelTextMessage>();
      msg->FromHeaderBuffer(header_data_);
      msg->FromBodyBuffer(body_data_);
      message = std::move(msg);
      break;
    }

    case ListenMessageType::TextMessage: {
      auto msg = std::make_unique<ListenTextMessage>();
      msg->FromHeaderBuffer(header_data_);
      msg->FromBodyBuffer(body_data_);
      message = std::move(msg);
      break;
    }

    case ListenMessageType::LogLevel: {
      auto msg = std::make_unique<LogLevelMessage>();
      msg->FromHeaderBuffer(header_data_);
      msg->FromBodyBuffer(body_data_);
      message = std::move(msg);
      break;
    }
    default: {
      LOG_ERROR() << "Unknown message type. Type: " << static_cast<int>(header.type_);
      break;
    }
  }
  if (message) {
    msg_queue_.Put(message);
  }
}

void ListenClient::Close() {
  if (socket_) {
    boost::system::error_code dummy;
    socket_->shutdown(ip::tcp::socket::shutdown_both, dummy);
    socket_->close(dummy);
  }
  socket_.reset();
}

} // end namespace
