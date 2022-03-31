/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "ods/odsitem.h"

namespace ods {

OdsItem::OdsItem(const std::string &name, const char* value)
: column_name_(name),
  value_(std::string(value != nullptr ? value : "")) {
}
template<>
std::string OdsItem::Value<std::string>() const {
  return value_;
}

template <>
[[nodiscard]] bool OdsItem::Value<bool>() const {
  if (value_.empty()) {
    return false;
  }
  switch (value_[0]) {
    case '1':
    case 't':
    case 'T':
    case 'y':
    case 'Y':
      return true;
    default:
      break;
  }
  return false;
}

template <>
void OdsItem::Value<std::string>(const std::string& value) {
  value_ = value;
}

template <>
void OdsItem::Value<double>(const double& value) {
  value_ = util::string::DoubleToString(value);;
}

template <>
void OdsItem::Value<float>(const float& value) {
  value_ = util::string::FloatToString(value);;
}

template <>
void OdsItem::Value<bool>(const bool& value) {
  value_ = value ? "1" : "0";
}


}