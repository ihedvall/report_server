/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <vector>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include <boost/process.hpp>
#pragma GCC diagnostic pop

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

std::string FindNotepad() {
  std::string note;
  // 1. Find the path to the 'notepad++.exe'
  try {
    std::vector< boost::filesystem::path > path_list = ::boost::this_process::path();
    path_list.emplace_back("c:/program files/notepad++");

    auto notepad = boost::process::search_path("notepad++", path_list);
    if (!notepad.string().empty()) {
      note = notepad.string();
    }
  } catch(const std::exception& ) {
    note.clear();
  }

  if (!note.empty()) {
    return note;
  }

  // 2. Find the path to the 'notepad.exe'
  try {
    auto notepad = boost::process::search_path("notepad");
    if (!notepad.string().empty()) {
      note = notepad.string();
    }
  } catch(const std::exception& ) {
    note.clear();
  }
  return note;
}
}


