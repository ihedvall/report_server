/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file xmlnode.h
 * \brief Implement an XML node.
 */
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <stack>
#include <expat.h>
#include "util/ixmlnode.h"
#include "util/ixmlfile.h"

namespace util::xml::detail {

/** \class XmlNode xmlnode.h "xmlnode.h"
 * \brief Implement an XML node
 */
 class XmlNode : public util::xml::IXmlNode {
 public:
  explicit XmlNode(const std::string &tag); ///< Constructor
  ~XmlNode() override = default; ///< Default destructor
  XmlNode() = delete; ///< Block usage of default constructor

  void AddNode(std::unique_ptr<XmlNode> p); ///< Adds new node
  void AppendData(const char *buffer, int len); ///< Append data to the node. Expat parser data.
  void SetAttribute(const XML_Char **attribute_list); ///< Sets the attributes. Expat parser data.

  const IXmlNode *GetNode(const std::string &tag) const override; ///< Returns a child node.
  void GetChildList(ChildList &child_list) const override; ///< Returns a list of child nodes.

 protected:
 private:
  using NodeList = std::vector<std::unique_ptr<util::xml::IXmlNode> >;
  NodeList node_list_;
};
}
