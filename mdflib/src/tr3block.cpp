/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "tr3block.h"

namespace {
constexpr size_t kIndexTx = 0;
} // end namespace

namespace mdf::detail {

std::string Tr3Block::Comment() const {
  return comment_;
}
size_t Tr3Block::Read(std::FILE *file) {
  size_t bytes = ReadHeader3(file);
  bytes += ReadLinks3(file, 1);
  bytes += ReadNumber(file, nof_events_);

  for (int event = 0; event < nof_events_; ++event) {
    if (bytes + 8 > block_size_) {
      break;
    }
    Tr3Event ev;
    bytes += ReadNumber(file, ev.event_time);
    bytes += ReadNumber<double>(file, ev.pre_time);
    bytes += ReadNumber(file, ev.post_time);
    event_list_.emplace_back(ev);
  }
  comment_ = ReadTx3(file,kIndexTx);
  return bytes;
}

size_t Tr3Block::Write(std::FILE *file) {
  // The TR block is normally added after the measurement has been stopped as it cannot be appended
  if (FilePosition() > 0) {
    return block_size_;
  }

  nof_events_ = static_cast<uint16_t>(event_list_.size());
  block_type_ = "TR";
  block_size_ = (2 + 2) + (1*4) + 2 + (nof_events_ * 24);
  link_list_.resize(1,0);

  if (!comment_.empty()) {
    Tx3Block tx(comment_);
    tx.Init(*this);
    tx.Write(file);
    link_list_[kIndexTx] = tx.FilePosition();
  }

  size_t bytes = IBlock::Write(file);
  bytes += WriteNumber(file, nof_events_);

  for (const auto& ev : event_list_) {
    bytes += WriteNumber(file, ev.event_time);
    bytes += WriteNumber(file, ev.pre_time);
    bytes += WriteNumber(file, ev.post_time);
  }

  return bytes;
}

}

