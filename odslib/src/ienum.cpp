/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "ods/ienum.h"

namespace ods {

void IEnum::AddItem(const std::string &item) {
  int64_t next_index = item_list_.empty() ? 0 : item_list_.crbegin()->first + 1;
  item_list_.insert({next_index, item});
}

}