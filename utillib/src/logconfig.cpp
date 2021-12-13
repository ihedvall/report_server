/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <mutex>
#include "util/logconfig.h"
#include "logfile.h"
#include "logconsole.h"

namespace util::log {

LogConfig &LogConfig::Instance() {
  static LogConfig log_config;
  return log_config;
}


void LogConfig::BaseName(const std::string &base_name) {
  std::lock_guard<std::mutex> lock(locker_);
  base_name_ = base_name;
}


const std::string &LogConfig::BaseName() const {
  std::lock_guard<std::mutex> lock(locker_);
  return base_name_;
}

void LogConfig::RootDir(const std::string &root_dir) {
  std::lock_guard<std::mutex> lock(locker_);
  root_dir_ = root_dir;
}

[[nodiscard]] const std::string &LogConfig::RootDir() const {
  std::lock_guard<std::mutex> lock(locker_);
  return root_dir_;
}

void LogConfig::SubDir(const std::string &sub_dir) {
  std::lock_guard<std::mutex> lock(locker_);
  sub_dir_ = sub_dir;
}

[[nodiscard]] const std::string &LogConfig::SubDir() const {
  std::lock_guard<std::mutex> lock(locker_);
  return sub_dir_;
}


bool LogConfig::CreateDefaultLogger() {
  bool create = false;

  switch (log_type_) {
    case LogType::LogToFile: {
      std::unique_ptr<detail::LogFile> log_file = std::make_unique<detail::LogFile>();
      create = !log_file->Filename().empty();
      if (create) {
        std::lock_guard<std::mutex> lock(locker_);
        log_chain_.emplace("Default", std::move(log_file));
      }
    }
      break;

    case LogType::LogToConsole:create = true;
      {
        std::unique_ptr<detail::LogConsole> log_console = std::make_unique<detail::LogConsole>();
        std::lock_guard<std::mutex> lock(locker_);
        log_chain_.emplace("Default", std::move(log_console));
      }
      break;

    case LogType::LogNothing:create = true;
      break;

    default:break;
  }
  return create;
}

LogConfig::~LogConfig() {
  DeleteLogChain();
}


void LogConfig::DeleteLogChain() {
  for (auto &itr: log_chain_) {
    itr.second->Stop();
  }
  log_chain_.clear();
}

void LogConfig::AddLogMessage(const LogMessage &message) const {
  if (!Enabled()) {
    return;
  }
  std::lock_guard<std::mutex> lock(locker_);
  for (auto &itr: log_chain_) {
    itr.second->AddLogMessage(message);
  }
}


void LogConfig::AddLogger(const std::string &logger_name, std::unique_ptr<ILogger> logger) {
  std::lock_guard<std::mutex> lock(locker_);
  auto itr = log_chain_.find(logger_name);
  if (itr == log_chain_.end()) {
    log_chain_.emplace(logger_name, std::move(logger));
  } else {
    itr->second = std::move(logger);
  }
}


void LogConfig::DeleteLogger(const std::string &logger_name) {
  std::lock_guard<std::mutex> lock(locker_);
  auto itr = log_chain_.find(logger_name);
  if (itr != log_chain_.end()) {
    log_chain_.erase(itr);
  }
}

ILogger *LogConfig::GetLogger(const std::string &logger_name) const {
  std::lock_guard<std::mutex> lock(locker_);
  const auto itr = log_chain_.find(logger_name);
  return itr == log_chain_.cend() ? nullptr : itr->second.get();
}


std::string LogConfig::GetLogFile(const std::string &logger_name) const {
  const auto itr1 = log_chain_.find(logger_name);
  if (itr1 != log_chain_.cend()) {
    return itr1->second->Filename();
  }
    // No default logger found. Do recover user mistake.
  for (const auto& itr2 : log_chain_) {
    if (itr2.second->HasLogFile()) {
      return itr2.second->Filename();
    }
  }
  return {};
}

void LogConfig::Type(LogType log_type) {
  log_type_ = log_type;
}
LogType LogConfig::Type() const {
  return log_type_;
}
void LogConfig::Enabled(bool enabled) {
  enabled_ = enabled;
}
bool LogConfig::Enabled() const {
  return enabled_;
}
}

