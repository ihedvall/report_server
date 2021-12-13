/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <iostream>
#include <filesystem>
#include "logconsole.h"
#include "util/timestamp.h"

namespace {
std::string GetStem(const std::string &file) {
  try {
    std::filesystem::path p(file);
    return p.stem().string();
  } catch (const std::exception &) {

  }
  return file;
}
}

namespace util::log::detail {
void LogConsole::AddLogMessage(const LogMessage &message) {
  if (message.message.empty()) {
    return;
  }
  const char last = message.message.back();
  const bool has_newline = last == '\n' || last == '\r';
  const std::string time = time::GetLocalTimestampWithMs(message.timestamp);
  const std::string severity = GetSeverityString(message.severity);

  std::lock_guard<std::mutex> guard(locker_); // Fix multi-thread issue

  std::clog << "[" << time << "] "
            << "[" << severity << "] "
            << "[" << GetStem(message.location.file_name()) << ":"
            << message.location.function_name()
            << ":" << message.location.line() << "] "
            << message.message;

  if (!has_newline) {
    std::clog << std::endl;
  }
  std::clog.flush();
}

void LogConsole::Stop() {

}

bool LogConsole::HasLogFile() const {
  return false;
}

std::string LogConsole::Filename() const {
  return std::string();
}

}