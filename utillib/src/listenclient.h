

#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <boost/asio.hpp>
#include "util/threadsafequeue.h"
#include "listenmessage.h"
namespace util::log::detail {
class ListenClient {
 public:
  ListenClient(const std::string& host_name, uint16_t port);
  virtual ~ListenClient();

  ListenClient() = delete;
  ListenClient(const ListenClient&) = delete;
  ListenClient& operator = (const ListenClient&) = delete;

  std::string HostName() const {
    return host_name_;
  }

  uint16_t Port() const {
    return port_;
  }

  std::string Name() const {
    return name_;
  }

  void Name(const std::string& name) {
    name_ = name;
  }

  std::string Description() const {
    return description_;
  }

  void Description(const std::string& description) {
    description_ = description;
  }

  bool Active() const {
    return active_;
  }

  void Active(bool active) {
    active_ = active;
  }

  bool IsConnected() const {
    return connected_;
  }

  bool GetMessage(std::unique_ptr<ListenMessage>& message) {
    return msg_queue_.Get(message, false);
  }

  void SendLogLevel(uint64_t level);
 private:
  std::string host_name_ = "127.0.0.1"; ///< By default set to local host.
  uint16_t    port_ = 0; ///< IP Port. Use port range 49152-65535.
  std::string name_; ///< Display name. Only used internally.
  std::string description_; ///< Description. Only used internally.
  std::atomic<bool> active_ = true; ///< Indicate if the message should be used or not
  std::atomic<bool> connected_ = false;

  boost::asio::io_context context_;
  boost::asio::ip::tcp::resolver lookup_;
  boost::asio::steady_timer retry_timer_;
  std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
  boost::asio::ip::tcp::resolver::results_type endpoints_;
  std::array<uint8_t, 8> header_data_ {0};
  std::vector<uint8_t> body_data_;
  ThreadSafeQueue<ListenMessage> msg_queue_;

  std::thread worker_thread_;
  void WorkerTask();
  void Close();
  void DoLookup();
  void DoConnect();
  void DoRetryWait();
  void DoReadHeader();
  void DoReadBody();

  void HandleMessage();


};

} // end namespace




