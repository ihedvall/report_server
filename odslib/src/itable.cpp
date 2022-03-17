/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include "ods/itable.h"
#include "util/stringutil.h"
namespace ods {

void ods::ITable::AddColumn(const IColumn& column) {
  column_list_.push_back(column);
}

void ITable::AddSubTable(const ITable& table) {
  sub_table_list_.insert({table.ApplicationId(), table});
}

const ITable *ITable::GetTable(int64_t application_id) const {
  if (application_id == ApplicationId()) {
    return this;
  }
  const auto itr = sub_table_list_.find(application_id);
  if (itr != sub_table_list_.cend()) {
    return &itr->second;
  }

  for (const auto& sub : sub_table_list_) {
    const auto* find = sub.second.GetTable(application_id);
    if (find != nullptr) {
      return find;
    }
  }
  return nullptr;
}

const ITable *ITable::GetBaseId(ods::BaseId base) const {
  if (BaseId() == base) {
    return this;
  }
  for (const auto& itr : sub_table_list_) {
    const auto* table = itr.second.GetBaseId(base);
    if (table != nullptr) {
      return table;
    }
  }
  return nullptr;
}

const ITable *ITable::GetTableByName(const std::string& name) const {
  if (util::string::IEquals(name, application_name_)) {
    return this;
  }
  for (const auto& itr : sub_table_list_) {
    const auto& table = itr.second;
    const auto* find = table.GetTableByName(name);
    if (find != nullptr) {
      return find;
    }
  }
  return nullptr;
}

const ITable *ITable::GetTableByDbName(const std::string& name) const {
  if (util::string::IEquals(name, database_name_)) {
    return this;
  }
  for (const auto& itr : sub_table_list_) {
    const auto& table = itr.second;
    const auto* find = table.GetTableByDbName(name);
    if (find != nullptr) {
      return find;
    }
  }
  return nullptr;
}

const IColumn *ITable::GetColumnByName(const std::string &name) const {
  const auto itr = std::ranges::find_if(column_list_, [&] (const auto& column) {
    return util::string::IEquals(name,column.ApplicationName());
  });
  return itr == column_list_.cend() ? GetColumnByDbName(name) : &(*itr);
}

const IColumn *ITable::GetColumnByDbName(const std::string &name) const {
   const auto itr = std::ranges::find_if(column_list_, [&] (const auto& column) {
    return util::string::IEquals(name,column.DatabaseName());
  });
  return itr == column_list_.cend() ? GetColumnByBaseName(name) : &(*itr);
}

const IColumn *ITable::GetColumnByBaseName(const std::string &name) const {
  const auto itr = std::ranges::find_if(column_list_, [&] (const auto& column) {
    return util::string::IEquals(name,column.BaseName());
  });
  return itr == column_list_.cend() ? nullptr : &(*itr);
}

} // end namespace

