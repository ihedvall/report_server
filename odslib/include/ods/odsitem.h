/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <string>
#include <typeinfo>
#include <util/stringutil.h>
#pragma once

namespace ods {

class OdsItem {
 private:
  std::string column_name_;
  std::string value_;

 public:
  OdsItem() = default;
  virtual ~OdsItem() = default;

  OdsItem(const std::string &name, const char* value);

  template <typename T>
  OdsItem(const std::string &name, const T& value)
    : column_name_(name) {
      Value(value);
  }

  [[nodiscard]] const std::string& ColumnName() const {
    return column_name_;
  }

  void ColumnName(const std::string& name) {
    column_name_ = name;
  }

  template <typename T>
  [[nodiscard]] T Value() const {
    try {
      std::istringstream temp(value_);
      T val {};
      temp >> val;
      return val;
    } catch (const std::exception& ) {

    }
    return T {};
  }

  template <typename T>
  void Value(const T& value) {
    try {
       value_ = std::to_string(value);
    } catch (const std::exception& ) {
      value_ = {};
    }
  }

};

template<>
std::string OdsItem::Value<std::string>() const;

template <>
[[nodiscard]] bool OdsItem::Value<bool>() const;

template <>
void OdsItem::Value<std::string>(const std::string& value);

template <>
void OdsItem::Value<double>(const double& value);

template <>
void OdsItem::Value<float>(const float& value);

template <>
void OdsItem::Value<bool>(const bool& value);
} // end namespace


