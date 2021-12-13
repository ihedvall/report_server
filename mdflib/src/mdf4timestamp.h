/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdio>
#include "iblock.h"
namespace mdf::detail {

namespace TimestampFlag {
constexpr uint8_t kLocalTimestamp = 0x01;
constexpr uint8_t kTimeOffsetValid = 0x02;
}

class Mdf4Timestamp : public IBlock {
 public:
  void GetBlockProperty(BlockPropertyList &dest) const override;
  size_t Read(std::FILE *file) override;
 private:
  uint64_t time_ = 0;      ///< Time in nanoseconds since 1970 also known as UNIX time
  int16_t tz_offset_ = 0;  ///< Time zone offsets in minutes. Best practice is to always use UTC.
  int16_t dst_offset_ = 0; ///< DST offset in minutes. Best practice is to always use UTC.
  uint8_t flags_ = 0;
};

} // Namespace mdf::detail



