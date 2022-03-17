/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <map>
#include <memory>
#include "util/timestamp.h"
#include "util/stringutil.h"
#include "util/ixmlnode.h"

#include "itable.h"
#include "ienum.h"

namespace ods {

class IModel {
 public:
  using TableList = std::map<int64_t, ITable>;
  using EnumList = std::map<std::string, IEnum, util::string::IgnoreCase>;

  IModel() = default;
  virtual ~IModel() = default;

  bool operator == (const IModel& model) const = default;

  [[nodiscard]] std::string Name() const {
    return name_;
  }
  void Name(const std::string& name) {
    name_ = name;
  }

  [[nodiscard]] std::string Version() const {
    return version_;
  }
  void Version(const std::string& version) {
    version_ = version;
  }

  [[nodiscard]] std::string Description() const {
    return description_;
  }
  void Description(const std::string& desc) {
    description_ = desc;
  }

  [[nodiscard]] std::string CreatedBy() const {
    return created_by_;
  }
  void CreatedBy(const std::string& creator) {
    created_by_ = creator;
  }

  [[nodiscard]] std::string ModifiedBy() const {
    return modified_by_;
  }
  void ModifiedBy(const std::string& creator) {
    modified_by_ = creator;
  }

  [[nodiscard]] std::string BaseVersion() const {
    return base_version_;
  }
  void BaseVersion(const std::string& version) {
    base_version_ = version;
  }

  [[nodiscard]] uint64_t Created() const {
    return created_;
  }
  void Created(uint64_t ns1970) {
    created_ = ns1970;
  }

  [[nodiscard]] uint64_t Modified() const {
    return modified_;
  }
  void Modified(uint64_t ns1970) {
    modified_ = ns1970;
  }

  void AddTable(const ITable& table);
  void AddEnum(const IEnum& obj);

  [[nodiscard]] const EnumList& Enums() const {
    return enum_list_;
  }
  [[nodiscard]] const TableList& Tables() const {
    return table_list_;
  }

  [[nodiscard]] const IEnum* GetEnum(const std::string& name) const;

  [[nodiscard]] const ITable* GetTable(int64_t application_id) const;
  [[nodiscard]] const ITable* GetTableByName(const std::string& name) const;
  [[nodiscard]] const ITable* GetTableByDbName(const std::string& name) const;
  [[nodiscard]] const ITable* GetBaseId(BaseId base) const;

  bool ReadModel(const std::string& filename);

 private:
  std::string name_ = "New model";
  std::string version_ = "1.0.0";
  std::string description_;

  std::string created_by_;
  std::string modified_by_;
  std::string base_version_ = "asam31";

  uint64_t created_ =  util::time::TimeStampToNs();
  uint64_t modified_ = util::time::TimeStampToNs();


  TableList table_list_;


  EnumList enum_list_;

  void ReadEnum(const util::xml::IXmlNode& node);
  void ReadTable(const util::xml::IXmlNode& node);
};

} // end namespace



