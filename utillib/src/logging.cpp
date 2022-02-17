/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "util/logging.h"
#include "util/logconfig.h"

namespace {

void SendLogMessage(const util::log::LogMessage &m) {
  const auto &log_config = util::log::LogConfig::Instance();
  log_config.AddLogMessage(m);
}

}
namespace util::log {

void LogDebug(const Loc &loc, const char *fmt, ...) {
  if (fmt == nullptr) {
    return;
  }
  char buffer[5000]{};
  std::va_list lsArg;
  va_start(lsArg, fmt);
  std::vsnprintf(buffer, 5000, fmt, lsArg);
  va_end(lsArg);
  buffer[4999] = '\0';

  LogString(loc, LogSeverity::kDebug, buffer);
}

void LogInfo(const Loc &loc, const char *fmt, ...) {
  if (fmt == nullptr) {
    return;
  }
  char buffer[5000]{};
  std::va_list lsArg;
  va_start(lsArg, fmt);
  std::vsnprintf(buffer, 5000, fmt, lsArg);
  va_end(lsArg);
  buffer[4999] = '\0';

  LogString(loc, LogSeverity::kInfo, buffer);
}

void LogError(const Loc &loc, const char *fmt, ...) {
  if (fmt == nullptr) {
    return;
  }
  char buffer[5000]{};
  std::va_list lsArg;
  va_start(lsArg, fmt);
  std::vsnprintf(buffer, 5000, fmt, lsArg);
  va_end(lsArg);
  buffer[4999] = '\0';

  LogString(loc, LogSeverity::kError, buffer);
}

void LogString(const Loc &loc, LogSeverity severity, const std::string &message) {
  LogMessage m;
  m.message = message;
  m.location = loc;
  m.severity = severity;

  SendLogMessage(m);
}
}


