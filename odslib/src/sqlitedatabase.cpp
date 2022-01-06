/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include "sqlitedatabase.h"
#include "util/logstream.h"

using namespace std::chrono_literals;
using namespace util::log;

namespace {

int BusyHandler(void* user, int nof_locks) {
  if (nof_locks < 1000) {
    std::this_thread::sleep_for(10ms);
    return 1;
  }
  return 0;
}

} // end namespace

namespace ods::detail {

SqliteDatabase::SqliteDatabase(const std::string &filename) {
  try {
    std::filesystem::path file(filename);
    file.make_preferred();
    filename_ = file.string();
  } catch (const std::exception& error) {
    LOG_ERROR() << error.what();
  }

}

SqliteDatabase::~SqliteDatabase() {
  Close(false);
}

bool SqliteDatabase::Open() {
  bool open = false;
  try {
    if (database_ == nullptr) {
      if (!std::filesystem::exists(filename_) ) {
        std::ostringstream err;
        err << "Database file does not exist. File: " << filename_;
        throw std::runtime_error(err.str());
      }

      int open3;
      size_t locks = 0;
      for (open3 = sqlite3_open_v2(filename_.c_str(), &database_, SQLITE_OPEN_READWRITE, nullptr);
           open3 == SQLITE_BUSY && locks < 1000;
           open3 = sqlite3_open(filename_.c_str(), &database_) ) {
          std::this_thread::sleep_for(10ms);
      }

      if (open3 != SQLITE_OK) {
        if (database_ != nullptr) {
          const auto error = sqlite3_errmsg(database_);
          sqlite3_close_v2(database_);
          database_ = nullptr;
          LOG_ERROR() << "Failed to open the database. Error: " << error << ", File: " << filename_;
        } else {
          LOG_ERROR() << "Failed to open the database. File: " << filename_;
        }
      }
    }

    if (database_ != nullptr) {
      sqlite3_busy_handler(database_,BusyHandler, this);
      ExecuteSql("PRAGMA foreign_keys = ON");
      ExecuteSql("BEGIN TRANSACTION");
      transaction_ = true;
      open = true;
    }
  } catch (const std::exception& error) {
    Close(false);
    LOG_ERROR() << error.what();
  }
  return open;
}

bool SqliteDatabase::OpenEx(int flags) {
  const auto open3 = sqlite3_open_v2(filename_.c_str(), &database_, flags, nullptr);
  if (open3 != SQLITE_OK) {
    if (database_ != nullptr) {
      const auto error = sqlite3_errmsg(database_);
      sqlite3_close_v2(database_);
      database_ = nullptr;
      LOG_ERROR() << "Failed to open the database. Error: " << error << ", File: " << filename_;
    } else {
      LOG_ERROR() << "Failed to open the database. File: " << filename_;
    }
  }
  return open3 == SQLITE_OK;
}

bool SqliteDatabase::Close(bool commit) {
  if (database_ == nullptr) {
    return true;
  }
  if (transaction_) {
    try {
      ExecuteSql(commit ? "COMMIT" : "ROLLBACK");
    } catch (const std::exception& error) {
      LOG_ERROR() << "Ending transaction failed. Error:" << error.what();
    }
    transaction_ = false;
  }

  const auto close = sqlite3_close_v2(database_);
  if (close != SQLITE_OK && database_ != nullptr) {
    LOG_ERROR() << "Failed to close the database. Error: " << sqlite3_errmsg(database_) << ", File: " << filename_;
  }
  database_ = nullptr;
  return close == SQLITE_OK;
}

bool SqliteDatabase::IsOpen() const {
  return database_ != nullptr;
}

void SqliteDatabase::ExecuteSql(const std::string &sql) {
  if (database_ == nullptr) {
    throw std::runtime_error("Database not open");
  }
  char* error = nullptr;
  const auto exec = sqlite3_exec(database_, sql.c_str(), nullptr, nullptr, &error);
  if (error != nullptr) {
    std::ostringstream err;
    err << "SQL Execute error. Error: " << error << ", SQL:" << sql;
    sqlite3_free(error);
    LOG_ERROR() << err.str();
    throw std::runtime_error(err.str());
  }
}

sqlite3 *SqliteDatabase::Sqlite3() {
  return database_;
}

}

