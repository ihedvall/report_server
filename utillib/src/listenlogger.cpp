/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "listenlogger.h"
#include "util/timestamp.h"
namespace util::log::detail {

ListenLogger::ListenLogger() :
    listen_proxy_("LISLOG") {
}

void ListenLogger::AddLogMessage(const LogMessage &message) {
  if (!listen_proxy_.IsActive()) {
    return;
  }
  // Log Level 0 = Show all, 1 = Show Debug.., 2 = Show Info..
  const auto log_level = listen_proxy_.LogLevel();
  const auto severity = static_cast<size_t>(message.severity);
  if (log_level > severity) {
    return;
  }
  const auto time = util::time::TimeStampToNs(message.timestamp);
  const std::string pre_text = util::log::GetSeverityString(message.severity);
  std::ostringstream temp;
  temp << message.message << " [" << message.location.function_name() << "]";
  listen_proxy_.AddMessage(time, pre_text, temp.str());
}

void ListenLogger::Stop() {
}

bool ListenLogger::HasLogFile() const {
  return false;
}

std::string ListenLogger::Filename() const {
  return std::string();
}

}

