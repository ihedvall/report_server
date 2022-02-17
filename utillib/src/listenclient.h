

#pragma once

#include <string>
#include <thread>
#include <boost/asio.hpp>
#include "util/threadsafequeue.h"
#include "listenmessage.h"
namespace util::log::detail {
class ListenClient {
 public:
  ListenClient(std::string  host_name, uint16_t port);
  virtual ~ListenClient();

  ListenClient() = delete;
  ListenClient(const ListenClient&) = delete;
  ListenClient& operator = (const ListenClient&) = delete;
 private:
  std::string host_name_ = "localhost";
  uint16_t    port_ = 0;

  boost::asio::io_context context_;
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




