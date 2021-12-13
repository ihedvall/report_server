/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include "util/ixmlfile.h"
#include "util/logstream.h"
#include "expatxml.h"

namespace util::xml {

std::string IXmlFile::FileNameWithoutPath() const {
  try {
    std::filesystem::path p(filename_);
    return p.filename().string();
  } catch (const std::exception &error) {
    util::log::LOG_ERROR()
        << "Invalid path. File: " << filename_
        << ", Error: " << error.what();
  }
  return filename_;
}

void IXmlFile::Reset() {
  root_node_.reset();
  version_ = {};
  encoding_ = "UTF-8";
  standalone_ = true;
}
void IXmlFile::FileName(const std::string &filename) {
  filename_ = filename;
}
std::string IXmlFile::FileName() const {
  return filename_;
}
std::string IXmlFile::Version() const {
  return version_;
}
std::string IXmlFile::Encoding() const {
  return encoding_;
}
bool IXmlFile::Standalone() const {
  return standalone_;
}
std::string IXmlFile::RootName() const {
  return root_node_ ? root_node_->TagName() : "";
}
void IXmlFile::GetChildList(IXmlNode::ChildList &child_list) const {
  if (root_node_) {
    root_node_->GetChildList(child_list);
  }
}
const IXmlNode *IXmlFile::GetNode(const std::string &tag) const {
  return root_node_ ? root_node_->GetNode(tag) : nullptr;
}

std::unique_ptr<IXmlFile> CreateXmlFile(const std::string & /* type */) {
  return std::make_unique<detail::ExpatXml>();
}

} // end namespace util::xml

