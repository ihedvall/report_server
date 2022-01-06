/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <string>
#include <sqlite3.h>
#include "ods/idatabase.h"

namespace ods::detail {

class SqliteDatabase : public IDatabase {
 public:
  SqliteDatabase() = delete;
  SqliteDatabase(const std::string& filename);
  ~SqliteDatabase() override;

  bool Open() override;
  bool OpenEx(int flags = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE);
  bool Close(bool commit) override;
  bool IsOpen() const override;

  void ExecuteSql(const std::string& sql);
  sqlite3* Sqlite3();

 private:
  std::string filename_;
  sqlite3* database_ = nullptr;
  bool transaction_ = false;
};

} // end namespace




