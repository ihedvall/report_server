/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include <fstream>
#include "util/ixmlfile.h"
#include "util/logstream.h"
#include "expatxml.h"
#include "xmlnode.h"
#include "writexml.h"

using namespace util::log;

namespace util::xml {

std::string IXmlFile::FileNameWithoutPath() const {
  try {
    std::filesystem::path p(filename_);
    return p.filename().string();
  } catch (const std::exception &error) {
    LOG_ERROR() << "Invalid path. File: " << filename_
        << ", Error: " << error.what();
  }
  return filename_;
}

bool IXmlFile::ParseFile() {
  return false;
}

bool IXmlFile::ParseString(const std::string &input) {
  return false;
}

void IXmlFile::Reset() {
  root_node_.reset();
  version_ = "1.0";
  encoding_ = "UTF-8";
  standalone_ = true;
}


void IXmlFile::GetChildList(IXmlNode::ChildList &child_list) const {
  if (root_node_) {
    root_node_->GetChildList(child_list);
  }
}

const IXmlNode *IXmlFile::GetNode(const std::string &tag) const {
  return root_node_ ? root_node_->GetNode(tag) : nullptr;
}

IXmlNode& IXmlFile::RootName(const std::string &name) {
  if (!root_node_) {
    root_node_ = CreateNode(name);
  } else {
    root_node_->TagName(name);
  }
  return *root_node_;
}

std::unique_ptr<IXmlNode> IXmlFile::CreateNode(const std::string& name) {
  return std::make_unique<detail::XmlNode>(name);
}

bool IXmlFile::WriteFile() {
  if (filename_.empty()) {
    LOG_ERROR() << "No file name defined";
    return false;
  }
  try {
    std::ofstream file(filename_.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!file.is_open()) {
      LOG_ERROR() << "Couldn't open file for writing. File: " << filename_;
      return false;
    }

    // Start with byte order characters if the encoding is UTF-8
    if (util::string::IEquals(encoding_, "UTF-8")) {
      file << "\xEF\xBB\xBF";
    }
    WriteRoot(file);

    file.close();

  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to write the file. Error: " << err.what() << ", File: " << FileName();
    return false;
  }
  return true;
}

std::string IXmlFile::WriteString() {
  std::ostringstream temp;
  WriteRoot(temp);
  return temp.str();
}

void IXmlFile::WriteRoot(std::ostream &dest) {
  dest << "<?xml version='" << Version() << "' encoding='" << Encoding() << "'";
  if (Standalone()) {
    dest << " standalone='yes'";
  }
  dest << "?>" << std::endl;

  if (!style_sheet_.empty()) {
    dest << "<?xml-stylesheet type='text/xsl' href='" << StyleSheet() << "'?>" << std::endl;
  }

  if (root_node_) {
    root_node_->Write(dest, 0);
  }
}


std::unique_ptr<IXmlFile> CreateXmlFile(const std::string &type) {
  if (util::string::IEquals("FileWriter", type)) {
    return std::make_unique<detail::WriteXml>();
  }
  return std::make_unique<detail::ExpatXml>();
}

} // end namespace util::xml

