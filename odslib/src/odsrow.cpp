/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include "ods/odsrow.h"
#include <util/stringutil.h>

using namespace util::string;

namespace ods {

OdsRow::OdsRow(const std::string &app_name)
: application_name_(app_name) {

}

const OdsItem *OdsRow::GetItem(const std::string &name) const {
  const auto itr = std::ranges::find_if(item_list_, [&] (const auto& item) {
    return IEquals(name, item.ColumnName());
  });
  return itr == item_list_.cend() ? nullptr : &*itr;
}

} // end namespace