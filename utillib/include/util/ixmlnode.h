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
#include <memory>
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
      || util::string::IEquals(v, "T", 1) // True/False
      || util::string::IEquals(v, "Y", 1); // Yes/No
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

  /** \brief Returns the nodes tag name.
   *
   * Return the XML nodes tag name.
   * @return Teturns the tag name.
   */
  [[nodiscard]] std::string TagName() const {
     return tag_name_;
   }

   /** \brief Sets the tag name.
    *
    * Sets the node tag name.
    * @param name The new tag name.
    */
   void TagName(const std::string& name) {
    tag_name_ = name;
  }

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
  [[nodiscard]] T Attribute(const std::string &key, const T &def = {}) const {
    for (const auto &p: attribute_list_) {
      if (util::string::IEquals(p.first, key)) {
        return XValue<T>(p.second);
      }
    }
    return def;
  }

   template<typename T>
   void SetAttribute(const std::string &key, const T &value) {
     const auto val = std::to_string(value);
     attribute_list_.insert({key, val});
   }

   template<typename T = bool>
   void SetAttribute(const std::string &key, const bool& value) {
     const auto val = std::to_string(value);
     attribute_list_.insert({key, value ? "yes" : "now"});
   }

   template<typename T = std::string>
   void SetAttribute(const std::string &key, const std::string& value) {
     attribute_list_.insert({key, value });
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

  template<typename T>
  void Value(const T& value) {
    value_ = std::to_string(value);
  }

  template<typename T = bool>
  void Value(const std::string& value) {
     value_ = value;
  }

  template<typename T = std::string>
  void Value(const bool& value) {
     value_ = value ? "yes" : "no";
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
  [[nodiscard]] T Property(const std::string &key, const T &def = {}) const {
    const IXmlNode *node = GetNode(key);
    return node != nullptr ? node->Value<T>() : def;
  }

  template<typename T>
  void SetProperty(const std::string &key, const T &value) {
    auto& node = AddNode(key);
    node.Value(value);
   }

  /** \typedef ChildList
   * \brief List of pointer to child nodes.
   */
  using ChildList = std::vector<const IXmlNode *>;
  virtual void GetChildList(ChildList &child_list) const; ///< Returns the child nodes.
  virtual const IXmlNode *GetNode(const std::string &tag) const; ///< Returns a node if it exist.

  /** \brief Returns true if the named property exist.
   *
   * Returns true if the named property exists. This function is normally used when deciding if
   * an attribute or a property key/value should be used.
   * @param tag The name of the key (tag).
   * @return True if it exist
   */
  [[nodiscard]] bool ExistProperty(const std::string& tag) const {
    return GetNode(tag) != nullptr;
  }

  virtual IXmlNode& AddNode(const std::string& name);
  virtual void AddNode(std::unique_ptr<IXmlNode> p);

  virtual void Write(std::ostream& dest, size_t level);
 protected:
  std::string tag_name_; ///< Name of this tag.
  std::string value_; ///< String value of this tag.

   /** \typedef AttributeList
    * \brief Indexed list of to key, attribute value pair.
    */
   using AttributeList = std::unordered_map<std::string, std::string>;
   AttributeList attribute_list_; ///< Indexed list of attribute

   using NodeList = std::vector<std::unique_ptr<IXmlNode> >;
   NodeList node_list_;


   virtual std::unique_ptr<IXmlNode> CreateNode(const std::string& name) const;
};
}
