/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <cstring>
#include "util/ixmlnode.h"
#include "util/stringutil.h"

namespace util::xml {
IXmlNode::IXmlNode(const std::string &tag_name)
    : tag_name_(tag_name) {
  util::string::Trim(tag_name_);
}


bool IXmlNode::IsTagName(const std::string &tag) const {
  if (string::IEquals(tag, tag_name_)) {
    return true;
  }
    // try the tag name without namespace
  const auto* ns = strchr(tag_name_.c_str(), ':');
  if (ns != nullptr) {
    ++ns;
    if (string::IEquals(tag, ns)) {
      return true;
    }
  }
  return false;
}

std::string IXmlNode::TagName() const {
  return tag_name_;
}

}
