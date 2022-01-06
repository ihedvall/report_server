/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <boost/endian/buffers.hpp>
#include "listenmessage.h"

using namespace boost::endian;

namespace util::log::detail {

SharedListenQueue::SharedListenQueue()
    : message_semaphore(0) {
}

void ListenMessage::ToBuffer(std::vector<uint8_t> &dest) {
  if (dest.size() < 8 + body_size_) {
    dest.resize(8 + body_size_,0);
  }
  little_uint16_buf_at type(static_cast<uint16_t>(type_));
  little_uint16_buf_at version(version_);
  little_uint32_buf_at body_size(body_size_);

  memcpy(dest.data(), type.data(), 2);
  memcpy(dest.data() + 2, version.data(), 2);
  memcpy(dest.data() + 4, body_size.data(), 4);
}

void ListenMessage::FromHeaderBuffer(const std::array<uint8_t,8> &source) {
  little_uint16_buf_at type;
  little_uint16_buf_at version;
  little_uint32_buf_at body_size;

  memcpy(type.data(), source.data(), 2);
  memcpy(version.data(), source.data() + 2, 2);
  memcpy(body_size.data(), source.data() + 4, 4);

  type_ = static_cast<ListenMessageType>(type.value());
  version_ = version.value();
  body_size_ = body_size.value();
}

LogLevelTextMessage::LogLevelTextMessage() {
  type_ = ListenMessageType::LogLevelText;
}

void LogLevelTextMessage::ToBuffer(std::vector<uint8_t> &dest) {
  uint32_t index = 8 + 8;
  for (const auto& itr : log_level_text_list_) {
    index += sizeof(uint64_t);
    index += 8 + itr.second.size();
  }
  body_size_ = index - 8;
  dest.resize(index, 0);

  ListenMessage::ToBuffer(dest);

  index = 8;
  little_uint64_buf_at vector_size(log_level_text_list_.size());
  memcpy(dest.data() + index, vector_size.data(),8);
  index += 8;

  for (const auto& itr1 : log_level_text_list_) {
    little_uint64_buf_at level(itr1.first);
    memcpy(dest.data() + index, level.data(),8);
    index += 8;

    little_uint64_buf_at text_size(itr1.second.size());
    memcpy(dest.data() + index, text_size.data(),8);
    index += 8;

    if (!itr1.second.empty()) {
      memcpy(dest.data() + index, itr1.second.data(), itr1.second.size());
      index += itr1.second.size();
    }
  }
}

void LogLevelTextMessage::FromBodyBuffer(const std::vector<uint8_t> &source) {
   if (source.size() < body_size_) {
    return;
  }
  uint32_t index = 0;
  little_uint64_buf_at vector_size;
  memcpy(vector_size.data(),source.data() + index, 8);
  index += 8;

  log_level_text_list_.clear();
  for (size_t ind = 0; ind < vector_size.value(); ++ind) {

    little_uint64_buf_at level;
    memcpy(level.data(), source.data() + index, 8);
    index += 8;

    little_uint64_buf_at text_size;
    memcpy(text_size.data(), source.data() + index, 8);
    index += 8;

    if (text_size.value() == 0) {
      log_level_text_list_.insert({level.value(), std::string()});
    } else {
      std::string text(text_size.value(), '\0');
      memcpy(text.data(), source.data() + index, text_size.value());
      index += text_size.value();
      log_level_text_list_.insert({level.value(),text});
    }
  }
}

ListenTextMessage::ListenTextMessage() {
  type_ = ListenMessageType::TextMessage;
}

ListenTextMessage::ListenTextMessage(const SharedListenMessage &msg)
    : ns1970_(msg.ns1970),
      pre_text_(msg.pre_text),
      text_(msg.text) {
  type_ = ListenMessageType::TextMessage;
}

void ListenTextMessage::ToBuffer(std::vector<uint8_t> &dest) {
  uint32_t index = 8 + 8 + (8 + pre_text_.size()) + (8 + text_.size());
  body_size_ = index - 8;
  dest.resize(index, 0);

  ListenMessage::ToBuffer(dest);

  index = 8;

  little_uint64_buf_at ns1970(ns1970_);
  memcpy(dest.data() + index, ns1970.data(), 8);
  index += 8;

  little_uint64_buf_at pre_text_size(pre_text_.size());
  memcpy(dest.data() + index, pre_text_size.data(), 8);
  index += 8;

  if (!pre_text_.empty()) {
    memcpy(dest.data() + index, pre_text_.data(), pre_text_.size());
    index += pre_text_.size();
  }

  little_uint64_buf_at text_size(text_.size());
  memcpy(dest.data() + index, text_size.data(), 8);
  index += 8;

  if (!text_.empty()) {
    memcpy(dest.data() + index, text_.data(), text_.size());
    //index += text_.size();
  }
}


void ListenTextMessage::FromBodyBuffer(const std::vector<uint8_t> &source) {

  if (source.size() < body_size_) {
    return;
  }

  uint32_t index = 0;

  little_uint64_buf_at ns1970;
  memcpy(ns1970.data(),source.data() + index, 8);
  ns1970_ = ns1970.value();
  index += 8;

  {
    little_uint64_buf_at pre_text_size;
    memcpy(pre_text_size.data(), source.data() + index, 8);
    index += 8;

    if (pre_text_size.value() == 0) {
      pre_text_.clear();
    } else {
      pre_text_.resize(pre_text_size.value(), '\0');
      memcpy(pre_text_.data(), source.data() + index, pre_text_size.value());
      index += pre_text_size.value();
    }
  }
  {
    little_uint64_buf_at text_size;
    memcpy(text_size.data(), source.data() + index, 8);
    index += 8;

    if (text_size.value() == 0) {
      text_.clear();
    } else {
      text_.resize(text_size.value(), '\0');
      memcpy(text_.data(), source.data() + index, text_size.value());
      // index += text_size.value();
    }
  }
}



LogLevelMessage::LogLevelMessage() {
  type_ = ListenMessageType::LogLevel;
}

void LogLevelMessage::ToBuffer(std::vector<uint8_t> &dest) {
  body_size_ = 8;
  dest.resize(16, 0);

  ListenMessage::ToBuffer(dest);

  little_uint64_buf_at level(log_level_);
  memcpy(dest.data() + 8, level.data(), 8);
}

void LogLevelMessage::FromBodyBuffer(const std::vector<uint8_t> &source) {

  if (source.size() < body_size_) {
    return;
  }

  little_uint64_buf_at level;
  memcpy(level.data(),source.data(), 8);
  log_level_ = level.value();
}

}

