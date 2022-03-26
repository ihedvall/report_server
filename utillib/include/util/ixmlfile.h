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

  /** \brief Sets the file name (full path).
   *
   * @param filename File name with full path
   */
  void FileName(const std::string& filename) {
    filename_ = filename;
  }

  /** \brief Return the full path file name.
   *
   * @return Full path to the file name
   */
  [[nodiscard]] std::string FileName() const {
    return filename_;
  }

  [[nodiscard]] std::string FileNameWithoutPath() const; ///< File name without path.

  /** \brief Returns the XML file version.
   *
   * Returns the XML file version. Typical 1.0 or 1.1.
   * @return XML version.
   */
  [[nodiscard]] std::string Version() const {
    return version_;
  }

  /** \brief Sets the version of the XML file.
   *
   * Sets the XML file version. Set default to 1.0.
   * @param version XML version.
   */
  void Version(const std::string& version) {
    version_ = version;
  }

  /** \brief Return the character encoding of the XML file.
   *
   * Returns the character encoding of the XML file. Standard encoding is UTF-8.
   * @return XML encoding.
   */
  [[nodiscard]] std::string Encoding() const {
    return encoding_;
  }
  /** \brief Sets the XML file encoding.
   *
   * Sets the XML file encoding. By default the UTF-8 encoding is used.
   * @param encoding XML file encoding
   */
  void Encoding(const std::string& encoding) {
    encoding_ = encoding;
  }

  /** \brief Returns true is the XML file is stand-alone.
   *
   * @return True if the XML file is stand-alone.
   */
  [[nodiscard]] bool Standalone() const {
    return standalone_;
  }

   /** \brief Returns the name of the root tag.
    *
    * @return Name of the root tag
    */
  [[nodiscard]] std::string RootName() const { ///< Return the name of the root tag.
     return root_node_ ? root_node_->TagName() : "";
  }
  /** \brief Create the root tag in an XML file object.
   *
   * Creates the root tag and sets its tag name.
   * @param name Name of the root tag
   */
  IXmlNode& RootName(const std::string& name);

  /** \brief Returns the used style sheet
   *
   * Returns the used style sheet.
   * @return The style sheet
   */
  [[nodiscard]] std::string StyleSheet() const {
    return style_sheet_;
  }

  /** \brief Sets the style sheet.
   *
   * Sets the style sheet. Include full url path.
   * @param style_sheet URL to style sheet.
   */
  void StyleSheet(const std::string& style_sheet) {
    style_sheet_ = style_sheet;
  }

  /** \brief Returns an XML tags value.
   *
   * Returns a value of an XML tag excluding the root tag.
   * @tparam T Type can be string, numbers and boolean.
   * @param [in] key Tag name with the value.
   * @param [in] def Default value if not found.
   * @return Returns the tag value or default value.
   */
  template<typename T>
  [[nodiscard]] T Property(const std::string &key, const T &def = {}) const {
    return root_node_ ? root_node_->Property<T>(key, def) : def;
  }

  template<typename T>
  void SetProperty(const std::string &key, const T &value) {
   if (root_node_) {
     root_node_->SetProperty(key, value);
   }
  }

  void GetChildList(IXmlNode::ChildList &child_list) const; ///< Returns a list of XML tags (nodes).

  [[nodiscard]] const IXmlNode *GetNode(const std::string &tag) const; ///< Returns a tag node.

  virtual bool ParseFile(); ///< Parses the XML file.
  virtual bool ParseString(const std::string &input); ///< Parses a string instead of a file.
  virtual void Reset(); ///< Reset the internals i.e. ready for a new parsing.
  virtual bool WriteFile();
  std::string WriteString();
 protected:
  IXmlFile() = default; ///< Default constructor is hidden from external users.
  std::string filename_; ///< File name with full path.
  std::string version_ = "1.0"; ///< Version of the XML file.
  std::string encoding_ = "UTF-8"; ///< Encoding of strings in the XML file.
  std::string style_sheet_;

  bool standalone_ = true; ///< True of the file is standalone.
  std::unique_ptr<IXmlNode> root_node_; ///< Pointer to the root node.

  virtual std::unique_ptr<IXmlNode> CreateNode(const std::string& name);
 private:
  void WriteRoot(std::ostream& dest);
};
/** \brief Creates an XML object that either parse or write.
 *
 * Creates an XML object that either parse a file or string or saves the object to an XML
 * file.
 * @param type String that creates an implementation of an XML object. By default is the Expat parser
 *             used.
 *
 *             <table><caption>Implementations</caption>
 *             <th>Type</th><th>Description</th>
 *             <tr><td>Expat</td><td>Creates a Expat SAX non-validating parser. This is the default usages.</td></tr>
 *             <tr><td>FileWriter</td><td>Creates a simple XML object that saves the XML objects to a file.</td></tr>
 *             </table>
 *
 * @return A smart pointer to an XML object.
 */
[[nodiscard]] std::unique_ptr<IXmlFile> CreateXmlFile(const std::string &type = "Expat"); ///< Creates a XML parser
}


