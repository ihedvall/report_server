/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <cstring>
#include "util/ixmlnode.h"
#include "util/stringutil.h"
#include "xmlnode.h"

namespace {
  std::string XmlString(const std::string& text) {
    std::ostringstream xml_string;
    for (char in_char : text) {
      switch (in_char) {
        case '<':
          xml_string << "&lt;";
          break;

        case '>':
          xml_string << "&gt;";
          break;

        case '&':
          xml_string << "&amp;";
          break;

        case '\"':
          xml_string << "&quot;";
          break;

        case '\'':
          xml_string << "&apos;";
          break;

        default:
          xml_string << in_char;
          break;
      }
    }
    return xml_string.str();
  }

}
namespace util::xml {

IXmlNode::IXmlNode(const std::string &tag_name)
    : tag_name_(tag_name) {
  util::string::Trim(tag_name_);
}

void IXmlNode::AddNode(std::unique_ptr<IXmlNode> p) {
  node_list_.push_back(std::move(p));
}

IXmlNode &IXmlNode::AddNode(const std::string &name) {
  auto node = CreateNode(name);
  auto& ret = *node;
  AddNode(std::move(node));
  return ret;
}

std::unique_ptr<IXmlNode> IXmlNode::CreateNode(const std::string &name) const {
  return std::make_unique<detail::XmlNode>(name);
}

const IXmlNode *IXmlNode::GetNode(const std::string &tag) const {
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

void IXmlNode::GetChildList(IXmlNode::ChildList &child_list) const {
  for (const auto &p: node_list_) {
    if (!p) {
      continue;
    }
    child_list.push_back(p.get());
  }
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

void IXmlNode::Write(std::ostream &dest, size_t level) { //NOLINT

  for (size_t tab = 0; tab < level; ++tab) {
    dest << "  ";
  }
  dest << "<" << TagName();
  for (const auto& attr : attribute_list_) {
    dest << " " << attr.first << "='" << XmlString(attr.second) << "'";
  }
  if (node_list_.empty() && value_.empty()) {
    dest << "/>" << std::endl;
  } else if (node_list_.empty()){
    dest << ">" << XmlString(value_) << "</" << TagName() << ">" << std::endl;
  } else {
    dest << ">" << std::endl;
    for (const auto& node : node_list_) {
      if (!node) {
        continue;
      }
      node->Write(dest, level + 1);
    }
    for (size_t tab1 = 0; tab1 < level; ++tab1) {
      dest << "  ";
    }
    dest << "</" << TagName() << ">" << std::endl;
  }
}


}
