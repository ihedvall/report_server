/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file ixmlnode.h
 * Interface class that defines a XML node (tag).
 */
#pragma once

#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>
#include "util/stringutil.h"

namespace {
template<typename T>
T XValue(const std::string &value) {
  std::string v(value);
  util::string::Trim(v);
  std::istringstream temp(v);
  T out = {};
  temp >> out;
  return out;
}

template<>
bool XValue(const std::string &value) {
  std::string v(value);
  util::string::Trim(v);
  return util::string::IEquals(v, "1")
      || util::string::IEquals(v, "ON")
      || util::string::IEquals(v, "T", 1);
}

template<>
std::string XValue(const std::string &value) {
  std::string v(value);
  util::string::Trim(v);
  return v;
}

} // end namespace

namespace util::xml {

/** \class IXmlNode ixmlnode.h [util/ixmlnode.h]
 * \brief Interface class against a XML tag (node) in a XML file.
 *
 * The class is the main interface when an XML file is more than one level deep.
 * Simple configuration files normally only have a root tag including one or more
 * value tags (properties).
 *
 * Each tag have a tag name and none or more attributes. If the tag name is unique
 * within the file, there is no need for attributes but if many tags with the same
 * name, it's recommended to set a unique attribute on each tag.
 *
 */
 class IXmlNode {
 public:
  explicit IXmlNode(const std::string &tag_name); ///< Creates a XML tag.
  virtual ~IXmlNode() = default; ///< Destructor

  IXmlNode() = delete;

  [[nodiscard]] std::string TagName() const; ///< Returns the tag name.

   /** \brief Returns true if the tag name match.
  * Check if this is a specific tag name. The check ignore case and namespaces.
  *
  * Example: If Tag name is '<ns:Foo>', IsTagName("foo") -> true
  * @param [in] tag Tag name
  * @return True if the tag name match.
  */
  [[nodiscard]] bool IsTagName(const std::string& tag) const;

  /** \brief Returns an attribute.
   *
   * Returns an attribute
   * @tparam T Type of value. Can be string, numbers or boolean.
   * @param [in] key Attribute name.
   * @param [in] def Default value if attribute is missing.
   * @return Returns the attribute value or default value..
   */
  template<typename T>
  T Attribute(const std::string &key, const T &def = {}) const {
    for (const auto &p: attribute_list_) {
      if (util::string::IEquals(p.first, key)) {
        return XValue<T>(p.second);
      }
    }
    return def;
  }

  /** \brief Returns a value.
   *
   * Returns the tag value
   * @tparam T Type of value. Can be string, numbers or boolean.
   * @return Returns the tag value
   */
  template<typename T>
  T Value() const {
    return XValue<T>(value_);
  }

  /** \brief Returns a specific tag value.
   *
   * Return a child tag value. Note that the Value() function returns this tags value.
   * @tparam T Type of value. Can be string, numbers or boolean.
   * @param [in] key Child tag name.
   * @param [in] def Default value.
   * @return The child tags value.
   */
  template<typename T>
  T Property(const std::string &key, const T &def = {}) const {
    const IXmlNode *node = GetNode(key);
    return node != nullptr ? node->Value<T>() : def;
  }
  /** \typedef ChildList
   * \brief List of pointer to child nodes.
   */
  using ChildList = std::vector<const IXmlNode *>;
  virtual void GetChildList(ChildList &child_list) const = 0; ///< Returns the child nodes.
  virtual const IXmlNode *GetNode(const std::string &tag) const = 0; ///< Returns a node if it exist.
 protected:
  std::string tag_name_; ///< Name of this tag.
  std::string value_; ///< String value of this tag.

   /** \typedef AttributeList
    * \brief Indexed list of to key, attribute value pair.
    */
   using AttributeList = std::unordered_map<std::string, std::string>;
  AttributeList attribute_list_; ///< Indexed list of attribute
};
}
