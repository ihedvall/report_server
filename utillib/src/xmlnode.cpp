/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <sstream>
#include "xmlnode.h"

namespace util::xml::detail {

void XmlNode::AddNode(std::unique_ptr<XmlNode> p) {
  node_list_.push_back(std::move(p));
}

void XmlNode::AppendData(const char *buffer, int len) {
  if (buffer == nullptr || len <= 0) {
    return;
  }
  std::ostringstream temp;
  for (int ii = 0; ii < len; ++ii) {
    if (buffer[ii] != '\0') {
      temp << buffer[ii];
    }
  }
  value_ += temp.str();
}

XmlNode::XmlNode(const std::string &tag_name)
    : IXmlNode(tag_name) {

}

void XmlNode::SetAttribute(const XML_Char **attribute_list) {
  attribute_list_.clear();
  if (attribute_list == nullptr) {
    return;
  }

  std::string key;
  for (int ii = 0; attribute_list[ii] != nullptr; ++ii) {
    if ((ii % 2) == 0) {
      key = attribute_list[ii];
    } else {
      std::string value = attribute_list[ii];
      attribute_list_.insert({key, value});
    }
  }
}

const IXmlNode *XmlNode::GetNode(const std::string &tag) const {
  for (const auto &p: node_list_) {
    if (!p) {
      continue;
    }
    if (p->IsTagName(tag)) {
      return p.get();
    }
  }
  return nullptr;
}

void XmlNode::GetChildList(IXmlNode::ChildList &child_list) const {
  for (const auto &p: node_list_) {
    if (!p) {
      continue;
    }
    child_list.push_back(p.get());
  }
}
}
