/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file logfile.h
 * \brief Implement a logger that saves the log message to a file.
 */
#pragma once

#include <string>
#include <cstdio>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include "util/logmessage.h"
#include "util/ilogger.h"

namespace util::log::detail {

/** \class LogFile logfile.h "logfile.h"
 * \brief Implement a logger that saves log messages to a file.
 *
 * Logger that saves the messages to file. The file has a rotating backup of the last 10 files.
 * The logger have an internal message queue so it will not delay the application.
 */
class LogFile final : public ILogger {
 public:
  LogFile() noexcept; ///< Constructor
  ~LogFile() override;///< Destructor

  LogFile(const LogFile &) = delete;
  LogFile(LogFile &&) = delete;
  LogFile &operator=(const LogFile &) = delete;
  LogFile &operator=(LogFile &&) = delete;


  std::string Filename() const override; ///< Returns the full path file name.

  void AddLogMessage(const LogMessage &message) override; ///< Handle a log message
  bool HasLogFile() const override; ///< Return true.

  void Stop() override; ///< Stops the working thread.

 private:
  std::FILE *file_ = nullptr;
  std::string filename_;
  std::mutex locker_;

  std::queue<LogMessage> message_list_;
  std::thread worker_thread_;
  std::atomic<bool> stop_thread_ = false;
  std::condition_variable condition_;

  void InitLogFile();
  void StartWorkerThread();
  void WorkerThread();
  void HandleMessage(const LogMessage &m);

};
}
