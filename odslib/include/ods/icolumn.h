/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include "odsdef.h"
namespace ods {

class IColumn {
 public:
  IColumn() = default;
  virtual ~IColumn() = default;

  bool operator == (const IColumn& column) const = default;

  [[nodiscard]] int64_t TableId() const {
    return table_id_;
  }
  void TableId(int64_t id) {
    table_id_ = id;
  }

  [[nodiscard]] int64_t ColumnId() const {
    return column_id_;
  }
  void ColumnId(int64_t id) {
    column_id_ = id;
  }

  [[nodiscard]] int64_t ReferenceId() const {
    return reference_id_;
  }
  void ReferenceId(int64_t id) {
    reference_id_ = id;
  }

  [[nodiscard]] int64_t UnitIndex() const {
    return unit_index_;
  }
  void UnitIndex(int64_t index) {
    unit_index_ = index;
  }

  [[nodiscard]] int64_t AclIndex() const {
    return acl_index_;
  }
  void AclIndex(int64_t index) {
    acl_index_ = index;
  }

  [[nodiscard]] ods::DataType DataType() const {
    return data_type_;
  }
  void DataType(ods::DataType type) {
    data_type_ = type;
  }

  [[nodiscard]] size_t DataLength() const {
    return data_length_;
  }
  void DataLength(size_t length) {
    data_length_ = length;
  }

  [[nodiscard]] uint16_t Flags() const {
    return flags_;
  }
  void Flags(uint16_t flags) {
    flags_ = flags;
  }

  [[nodiscard]] std::string ApplicationName() const {
    return application_name_;
  }
  void ApplicationName(const std::string& name) {
    application_name_ = name;
  }

  [[nodiscard]] std::string BaseName() const {
    return base_name_;
  }
  void BaseName(const std::string& name) {
    base_name_ = name;
  }

  [[nodiscard]] std::string DatabaseName() const {
    return database_name_;
  }
  void DatabaseName(const std::string& name) {
    database_name_ = name;
  }

  [[nodiscard]] std::string ReferenceName() const {
    return reference_name_;
  }
  void ReferenceName(const std::string& name) {
    reference_name_ = name;
  }

  [[nodiscard]] std::string Description() const {
    return description_;
  }
  void Description(const std::string& desc) {
    description_ = desc;
  }

  [[nodiscard]] std::string DisplayName() const {
    return display_name_;
  }
  void DisplayName(const std::string& name) {
    display_name_ = name;
  }

  [[nodiscard]] std::string EnumName() const {
    return enum_name_;
  }
  void EnumName(const std::string& name) {
    enum_name_ = name;
  }

  [[nodiscard]] size_t NofDecimals() const {
    return nof_decimals_;
  }
  void NofDecimals(size_t decimals) {
    nof_decimals_ = decimals;
  }

  [[nodiscard]] std::string Unit() const {
    return unit_;
  }
  void Unit(const std::string& unit) {
    unit_ = unit;
  }

 private:
  int64_t table_id_ = 0;  ///< Reference to father table.
  int64_t column_id_ = 0; ///< Order number within the table.
  int64_t reference_id_ = 0; ///< Reference table ID. 0 == no reference.
  int64_t unit_index_ = 0; ///< Index to the unit table.
  int64_t acl_index_ = 0; ///< Index to the ACL table. Normally ignored.

  ods::DataType data_type_ = ods::DataType::DtUnknown; ///< Data type of the column
  size_t data_length_ = 0; ///< Normally set to 0. Optionally used by GUI to limit the string length.
  uint16_t flags_ = 0; ///< Various database flags
  size_t nof_decimals_ = 0; ///< Number of floating decimals. Optional information.

  std::string application_name_; ///< Model name of the column.
  std::string base_name_; ///< Base name if the column is defined in the ODS specification.
  std::string database_name_; ///< Database column name.
  std::string reference_name_; ///< All references normally refer to the auto index, so leave blank.
  std::string enum_name_; ///< Name of the enumerate.
  std::string display_name_; ///< User friendly name. Typical used to labels.Optional information.
  std::string description_; ///< Optional information.
  std::string unit_;
};

}


