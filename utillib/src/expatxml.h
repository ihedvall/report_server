/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file expatxml.h
 * \brief Implement a wrapper around the EXPAT parser.
 */
#pragma once
#include <memory>
#include <expat.h>
#include "util/ixmlfile.h"
#include "util/ixmlnode.h"

#include "xmlnode.h"

namespace util::xml::detail {
/** \class ExpatXml expatxml.h "expatxml.h"
 * \brief Simple wrapper around the EXPAT parser.
 *
 * Implements an XML parser by using the EXPAT parser.
 */
class ExpatXml final :  public util::xml::IXmlFile {
 public:
  ExpatXml() = default; ///< Constructor
  ~ExpatXml() override = default; ///< Destructor

  bool ParseFile() override; ///< Parses the input file.
  bool ParseString(const std::string &input) override; ///< Parses an input string.
  void Reset() override; ///< Reset the parser for a new round.


  void StartElement(const XML_Char *name, const XML_Char **attributes); ///< Parser callback
  void EndElement(const XML_Char *name); ///< Parser callback
  void CharacterData(const char *buffer, int len); ///< Parser callback
  void XmlDecl(const XML_Char *version,
                 const XML_Char *encoding,
                 int standalone); ///< Parser callback
 protected:
 private:
  using NodeStack = std::stack<XmlNode *>; ///< Temporary parser stack
  NodeStack node_stack_;
};
} // end namespace util::xml::detail

