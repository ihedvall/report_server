/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "idatabase.h"

namespace ods {

class DatabaseGuard final {
 public:

  DatabaseGuard(IDatabase& database);
  DatabaseGuard() = delete;
  ~DatabaseGuard();

  void Rollback();
  bool IsOk() const;
 private:
  IDatabase* database_ = nullptr;
  bool db_ok_ = false;
};


} // end namespace ods
