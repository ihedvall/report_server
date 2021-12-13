/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file ixmlfile.h
 * Interface class that defines a XML parser.
 */
#pragma once
#include <string>
#include <memory>
#include "util/ixmlnode.h"

namespace util::xml {

/** \class IXmlFile ixmlfile.h "util/ixmlfile.h"
 * \brief Interface class that defines an XML parser.
 *
 * The interface shall hide the implementation code of an XML parser.
 */
class IXmlFile {
 public:
  virtual ~IXmlFile() = default; ///< Destructor.

  void FileName(const std::string &filename); ///< Sets the file name with full path.

  [[nodiscard]] std::string FileName() const; ///< Returns the file name with full path.

  [[nodiscard]] std::string FileNameWithoutPath() const; ///< File name without path.

  [[nodiscard]] std::string Version() const; ///< Returns the XML version.

  [[nodiscard]] std::string Encoding() const; ///< Returns the encoding.

  [[nodiscard]] bool Standalone() const; ///< Returns true if it is standalone

  [[nodiscard]] std::string RootName() const; ///< Return the name of the root tag.

  /** \brief Returns an XML tags value.
   *
   * Returns a value of an XML tag excluding the root tag.
   * @tparam T Type can be string, numbers and boolean.
   * @param [in] key Tag name with the value.
   * @param [in] def Default value if not found.
   * @return Returns the tag value or default value.
   */
  template<typename T>
  T Property(const std::string &key, const T &def = {}) const {
    return root_node_ ? root_node_->Property<T>(key, def) : def;
  }

  void GetChildList(IXmlNode::ChildList &child_list) const; ///< Returns a list of XML tags (nodes).

  [[nodiscard]] const IXmlNode *GetNode(const std::string &tag) const; ///< Returns a tag node.

  virtual bool ParseFile() = 0; ///< Parses the XML file.
  virtual bool ParseString(const std::string &input) = 0; ///< Parses a string instead of a file.
  virtual void Reset(); ///< Reset the internals i.e. ready for a new parsing.
 protected:
  IXmlFile() = default; ///< Default constructor is hidden from external users.
  std::string filename_; ///< File name with full path.
  std::string version_; ///< Version of the XML file.
  std::string encoding_ = "UTF-8"; ///< Encoding of strings in the XML file.
  bool standalone_ = true; ///< True of the file is standalone.
  std::unique_ptr<IXmlNode> root_node_; ///< Pointer to the root node.

};

[[nodiscard]] std::unique_ptr<IXmlFile> CreateXmlFile(const std::string &type = "Expat"); ///< Creates a XML parser
}


