/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file logconsole.h
 * \brief Sends all log messages onto the console.
 */
#pragma once
#include <string>
#include <mutex>
#include "util/ilogger.h"

namespace util::log::detail {
/** \class LogConsole logconsole.h "logconsole.h"
 * \brief Implements a logger that sends the message to the console window.
 *
 * This class implements a logger that sends all log messages to the console.
 * This is useful for applications without GUI and where log files are annoying.
 *
 * Note that this implementations sends the message to the console directly thus
 * delaying the application.
 */
class LogConsole final : public ILogger {
 public:
  LogConsole() = default;
  ~LogConsole() override = default;

  LogConsole(const LogConsole &) = delete;
  LogConsole(LogConsole &&) = delete;
  LogConsole &operator=(const LogConsole &) = delete;
  LogConsole &operator=(const LogConsole &&) = delete;

  void AddLogMessage(const LogMessage &message) override; ///< Mandatory message interface
  [[nodiscard]] bool HasLogFile() const override; ///< Returns true if it have a log file.
  [[nodiscard]] std::string Filename() const override; ///< Returns the log file
  void Stop() override; ///< Stops any working thread.

 private:
  std::mutex locker_;
};
}


