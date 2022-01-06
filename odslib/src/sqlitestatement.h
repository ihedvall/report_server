/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "sqlitedatabase.h"
namespace ods::detail {

class SqliteStatement final {
 public:
  SqliteStatement(sqlite3* database, const std::string& sql);
  ~SqliteStatement();
  SqliteStatement() = delete;

  bool Step();
  void Reset();

  [[nodiscard]] bool IsNull(int column) const;


  void SetValue(int index, int64_t value) const;
  void SetValue(int index, double value) const;
  void SetValue(int index, const std::string& value) const;
  void SetValue(int index, const std::vector<uint8_t>& value) const;

  template<typename T>
  void GetValue(int column, T& value) const;

  template<typename T = std::string>
  void GetValue(int column, std::string& value) const;

  template<typename T = std::vector<uint8_t>>
  void GetValue(int column, std::vector<uint8_t>& value) const;

 private:
  sqlite3*  database_ = nullptr;
  sqlite3_stmt* statement_ = nullptr;
};

template<typename T>
void SqliteStatement::GetValue(int column, T& value) const {
  if (statement_ == nullptr) {
    throw std::runtime_error("Statement is null");
  }
  const auto type = sqlite3_column_type(statement_, column);
  switch (type) {
    case SQLITE_INTEGER: {
      const auto temp = sqlite3_column_int64(statement_, column);
      value = static_cast<T>(temp);
      break;
    }

    case SQLITE_FLOAT: {
      const auto temp = sqlite3_column_double(statement_, column);
      value = static_cast<T>(temp);
      break;
    }

    case SQLITE_TEXT: {
      const auto* temp = sqlite3_column_text(statement_, column);
      const auto bytes = sqlite3_column_bytes(statement_,column);
      std::stringstream val;
      for (int byte = 0; temp != nullptr && byte < bytes; ++byte) {
        val << static_cast<char>(temp[byte]);
      }
      val >> value;
      break;
    }

    case SQLITE_BLOB:
    case SQLITE_NULL:
    default:
      value = {};
      break;
  }
}

} // end namespace ods::detail


