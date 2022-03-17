/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>
namespace ods {

bool IsSqlReservedWord(const std::string& word);

class IDatabase {
 public:
  virtual ~IDatabase() = default;
//  virtual std::string Name() const = 0;

  virtual bool Open() = 0;
  virtual bool Close(bool commit = true) = 0;
  virtual bool IsOpen() const = 0;
 protected:
  IDatabase() = default;
 private:

};

} // end namespace ods
