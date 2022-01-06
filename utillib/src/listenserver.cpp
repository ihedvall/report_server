/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <chrono>
#include <boost/asio.hpp>
#include "util/logstream.h"
#include "listenserver.h"
#include "listenserverconnection.h"

using namespace util::log;
using namespace boost::asio;
using namespace std::chrono_literals;

namespace util::log::detail {

ListenServer::ListenServer()
: cleanup_timer_(context_),
  queue_timer_(context_) {
  Name("Listen");
}

ListenServer::ListenServer(const std::string &share_name)
: ListenServer() {
  ShareName(share_name);
}

ListenServer::~ListenServer() {
  if (!context_.stopped()) {
    context_.stop();
  }
  if (worker_thread_.joinable() ) {
   worker_thread_.join();
  }
}

void ListenServer::WorkerTask() {
  try {
    const auto& count = context_.run();
    LOG_INFO() << "Stopped main worker thread. Name: " << Name() << ", Count: " << count;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Context error. Name: " << Name() << ", Error: " << error.what();
  }
}
bool ListenServer::IsActive() const {
  return active_;
}

bool ListenServer::Start() {
  bool start = false;
  active_ = false;
  if (share_mem_queue_) {
    share_mem_queue_->SetActive(false);
  }
  try {
    const auto address = ip::make_address(HostName());
    ip::tcp::endpoint endpoint(address, Port());
    acceptor_ = std::make_unique<ip::tcp::acceptor>(context_,endpoint);
    DoAccept();
    DoCleanup();
    DoMessageQueue();
    worker_thread_ = std::thread(&ListenServer::WorkerTask, this);
    start = true;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Failed to start the server. Name: " << Name() << ", Error: " << error.what();
  }
  return start;
}

bool ListenServer::Stop() {
  bool stop = false;
  active_ = false;
  if (share_mem_queue_) {
    share_mem_queue_->SetActive(false);
  }
  try {
    if (!context_.stopped()) {
      context_.stop();
    }
    if (worker_thread_.joinable() ) {
      worker_thread_.join();
    }
    std::lock_guard lock(connection_list_lock_);
    connection_list_.clear();
    stop = true;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Failed to stop the server. Name: " << Name() << ", Error: " << error.what();
  }

  return stop;
}

void ListenServer::AddMessage(uint64_t nano_sec_1970, const std::string &pre_text, const std::string &text) {
  auto msg = std::make_unique<ListenTextMessage>();
  msg->ns1970_ = nano_sec_1970;
  msg->pre_text_ = pre_text;
  msg->text_ = text;
  InMessage(std::move(msg));
}

void ListenServer::DoAccept() {
  connection_socket_ = std::make_unique<ip::tcp::socket>(context_);
  acceptor_->async_accept(*connection_socket_, [&] (const boost::system::error_code& error) {
    if (error) {
      connection_socket_.reset();
      LOG_ERROR() << "Accept error. Name: " << Name() << ", Error: " << error.message();
    } else {
      auto connection = std::make_unique<ListenServerConnection>(*this, connection_socket_);
      {
        std::lock_guard lock(connection_list_lock_);
        connection_list_.push_back(std::move(connection));
      }
      active_ = true;
      if (share_mem_queue_) {
        share_mem_queue_->SetActive(active_);
      }
      LOG_INFO() << "Added a connection";
      DoAccept();
    }
  });
}

void ListenServer::DoCleanup() {
  cleanup_timer_.expires_after(10s);
  cleanup_timer_.async_wait([&] (const boost::system::error_code error) {
    if (error) {
      LOG_ERROR() << "Cleanup timer error. Name: " << Name() << ", Error: " << error.message();
    } else {
      std::lock_guard lock(connection_list_lock_);
      for (auto itr = connection_list_.begin(); itr != connection_list_.end(); /* No ++itr here */ ) {
        auto& connection = *itr;
        if (!connection || connection->Cleanup()) {
          LOG_INFO() << "Deleted a connection";
          itr = connection_list_.erase(itr);
        } else {
          ++itr;
        }
      }
      active_ = !connection_list_.empty();
      if (share_mem_queue_) {
        share_mem_queue_->SetActive(active_);
      }
      DoCleanup();
    }
  });

}

void ListenServer::DoMessageQueue() {
  queue_timer_.expires_after(50ms);
  queue_timer_.async_wait([&] (const boost::system::error_code error) {
    if (error) {
      LOG_ERROR() << "Message queue timer error. Name: " << Name() << ", Error: " << error.message();
    } else {
      if (share_mem_queue_) {
        SharedListenMessage share_msg;
        for (bool share = share_mem_queue_->Get(share_msg, false); share;
                  share = share_mem_queue_->Get(share_msg, false)) {
          auto text = std::make_unique<ListenTextMessage>(share_msg);
          InMessage(std::move(text));
        }
      }
      std::unique_ptr<ListenMessage> msg;
      for (bool message = msg_queue_.Get(msg, false);
          message;
          message = msg_queue_.Get(msg, false)) {
        HandleMessage(msg.get());
        msg.reset();
      }
      DoMessageQueue();
    }
  });

}

boost::asio::io_context &ListenServer::Context() {
  return context_;
}

void ListenServer::InMessage(std::unique_ptr<ListenMessage> msg) {
  msg_queue_.Put(msg);
}

void ListenServer::SetLogLevelText(size_t level, const std::string &menu_text) {
  log_level_list_.insert({level, menu_text});
}

const std::map<uint64_t, std::string> &ListenServer::LogLevelList() const {
  return log_level_list_;
}

size_t ListenServer::LogLevel() {
  return log_level_;
}

void ListenServer::ShareName(const std::string &share_name) {
  share_name_ = share_name;
  if (share_name_.empty()) {
    share_mem_queue_.reset();
  } else {
    share_mem_queue_ = std::make_unique<MessageQueue>(true, share_name_);
  }
}

std::string ListenServer::ShareName() const {
  return share_name_;
}

void ListenServer::HandleMessage(const ListenMessage* msg) {
  if (msg == nullptr) {
    return;
  }
  switch (msg->type_) {
    case ListenMessageType::LogLevel: {
      // Set new log level and send out to clients
      const auto* log_level = dynamic_cast<const LogLevelMessage*>(msg);
      if (log_level != nullptr) {
        log_level_ = log_level->log_level_;
        if (share_mem_queue_) {
          share_mem_queue_->SetLogLevel(log_level_);
        }        std::lock_guard lock(connection_list_lock_);
        for (auto& connection : connection_list_) {
          auto send_msg = std::make_unique<LogLevelMessage>(*log_level);
          connection->InMessage(std::move(send_msg));
        }
      }
      break;
    }

    case ListenMessageType::TextMessage: {
      // Send out to clients
      const auto* text = dynamic_cast<const ListenTextMessage*>(msg);
      if (text != nullptr) {
        std::lock_guard lock(connection_list_lock_);
        for (auto& connection : connection_list_) {
          auto send_msg = std::make_unique<ListenTextMessage>(*text);
          connection->InMessage(std::move(send_msg));
        }
      }
      break;
    }

    case ListenMessageType::LogLevelText:
    default:
      break;

  }
}


} // end namespace