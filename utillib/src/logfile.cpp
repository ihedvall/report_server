/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include <iostream>
#include <exception>
#include <chrono>

#include "logfile.h"
#include "util/logconfig.h"
#include "util/timestamp.h"
#include "util/logstream.h"

using namespace std::filesystem;
using namespace std::chrono_literals;

namespace {
std::string GetStem(const std::string &file) {
  try {
    std::filesystem::path p(file);
    return p.stem().string();
  } catch (const std::exception &) {

  }
  return file;
}

std::string FindLogPath() {
  try {
    auto &log_config = util::log::LogConfig::Instance();

    path filename(log_config.BaseName());

    // Add an extension if it is missing
    if (!filename.has_extension()) {
      filename.replace_extension(".log");
    }

    // If the user supply a full path, then use the path as it is
    if (filename.has_root_path()) {
      return filename.string();
    }
    path root_dir(log_config.RootDir());
    if (root_dir.empty()) {
      root_dir = temp_directory_path();
    }
    std::string sub_dir = log_config.SubDir();
    if (!sub_dir.empty()) {
      root_dir.append(sub_dir);
    }
    create_directories(root_dir);

    root_dir.append(filename.string());
    return root_dir.string();
  } catch (const std::exception &error) {
    std::cerr << "FindLogPath(). Error: " << error.what() << std::endl;
    throw error; // No meaning to log the error to file
  }
}

void BackupFiles(const std::string &filename) {
  if (filename.empty()) {
    return;
  }
  path f(filename);
  path p(f.parent_path());
  path s(f.stem());
  if (!exists(f)) {
    return; // no meaning to back up if original doesn't exist
  }
  // shift all file xxx_N -> xxx_N-1 and last xxx -> xxx_0
  for (int ii = 9; ii >= 0; --ii) {
    std::ostringstream temp1;
    temp1 << s.string() << "_" << ii << ".log";

    path f1(p);
    f1.append(temp1.str());
    if (exists(f1) && ii == 9) {
      remove(f1);
    }
    if (ii == 0) {
      rename(f, f1);
    } else {
      std::ostringstream temp2;
      temp2 << s.string() << "_" << ii - 1 << ".log";
      path f2(p);
      f2.append(temp2.str());
      if (exists(f2)) {
        rename(f2, f1);
      }
    }
  }
}
}

namespace util::log::detail {

std::string LogFile::Filename() const {
  return filename_;
}

LogFile::LogFile() noexcept {
  InitLogFile();
}

LogFile::~LogFile() {
  stop_thread_ = true;
  if (worker_thread_.joinable()) {
    condition_.notify_one();
    worker_thread_.join();
  }
  stop_thread_ = false;
  if (file_ != nullptr) {
    fclose(file_);
  }
}

void LogFile::StartWorkerThread() {
  stop_thread_ = false;
  worker_thread_ = std::thread(&LogFile::WorkerThread, this);
}

void LogFile::WorkerThread() {
  try {
    BackupFiles(filename_);
  } catch (const std::exception &error) {
    util::log::LOG_ERROR() << "Failed to backup log files at start. Error: " << error.what();
  }

  do {
    std::unique_lock<std::mutex> lock(locker_);
    condition_.wait_for(lock, file_ == nullptr ? 1000ms : 10000ms,
                        [&] {return stop_thread_.load();});

    int message_count = 0;
    for (; !message_list_.empty() && message_count <= 1000; ++message_count) {
      LogMessage m = message_list_.front();
      message_list_.pop();
      lock.unlock();
      HandleMessage(m);
      lock.lock();
    }
    if ((message_count == 0 || message_count >= 1000) && file_ != nullptr) {
      std::fclose(file_);
      file_ = nullptr;
    }
    try {
      path p(filename_);
      if (file_ == nullptr && exists(p) && file_size(p) > 10'000'000) {
        BackupFiles(filename_);
      }
    } catch (const std::exception &error) {
      util::log::LOG_ERROR() << "Failed to backup log files. Error: " << error.what();
    }
  } while (!stop_thread_);

  if (file_ != nullptr) {
    std::fclose(file_);
    file_ = nullptr;
  }
}

void LogFile::HandleMessage(const LogMessage &m) {
  if (file_ == nullptr) {
    file_ = fopen(filename_.c_str(), "at");
  }
  if (file_ == nullptr) {
    return;
  }
  if (m.message.empty()) {
    return;
  }
  const char last = m.message.back();
  const bool has_newline = last == '\n' || last == '\r';
  const std::string time = time::GetLocalTimestampWithMs(m.timestamp);
  const std::string severity = GetSeverityString(m.severity);

  std::ostringstream temp;
  temp << "[" << time << "] "
       << severity << " "
       << "[" << GetStem(m.location.file_name()) << ":"
       << m.location.function_name()
       << ":" << m.location.line() << "] "
       << m.message;

  if (!has_newline) {
    temp << std::endl;
  }
  std::fwrite(temp.str().data(), 1, temp.str().size(), file_);
}

/**
 * Adds a log message to the internal message queue. The queue is saved to a file by a worker thread.
 * @param [in] message Message to handle.
 */
void LogFile::AddLogMessage(const LogMessage &message) {
  if (stop_thread_) {
    return;
  }
  {
    std::lock_guard<std::mutex> lock(locker_);
    if (stop_thread_) {
      return;
    }
    message_list_.push(message);
  }
  condition_.notify_one();
}

/**
 * Stops the working thread. This means that all messages in the queue is saved to the file.
 */
void LogFile::Stop() {
  stop_thread_ = true;
  if (worker_thread_.joinable()) {
    condition_.notify_one();
    worker_thread_.join();
  }
  stop_thread_ = false;
  if (file_ != nullptr) {
    fclose(file_);
  }
}

void LogFile::InitLogFile() {
  try {
    stop_thread_ = true;
    if (worker_thread_.joinable()) {
      condition_.notify_one();
      worker_thread_.join();
    }
    stop_thread_ = false;
    if (file_ != nullptr) {
      fclose(file_);
      file_ = nullptr;
    }
    filename_ = {};
    path p = FindLogPath();
    if (p.empty()) {
      throw std::ios_base::failure(std::string("Path is empty. Path: ") + p.string());
    }
    if (!exists(p.parent_path())) {
      throw std::ios_base::failure(std::string("Path does not exist Path: ") + p.string());
    }
    filename_ = p.string();
    StartWorkerThread();
  } catch (const std::exception &error) {
    std::cerr << "Couldn't initiate a log file. Error: " << error.what() << std::endl;
  }

}

bool LogFile::HasLogFile() const {
  return !filename_.empty();
}

}
