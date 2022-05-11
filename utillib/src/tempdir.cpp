/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <filesystem>
#include <boost/filesystem.hpp>
#include "util/tempdir.h"
#include "util/logstream.h"

namespace util::log {

TempDir::TempDir(const std::string &sub_dir, bool unique_dir) {
  // Create a temporary directory for this application
  try {
    auto temp_dir = std::filesystem::temp_directory_path();
    if (unique_dir) {
      std::ostringstream temp;
      if (sub_dir.empty()) {
        temp << "utillib_%%%%";
      } else {
        temp << sub_dir << "_%%%%";
      }
      const auto unique = boost::filesystem::unique_path(temp.str());
      temp_dir.append(unique.string());
    } else {
      if (sub_dir.empty()) {
        temp_dir.append("utillib");
      } else {
        temp_dir.append(sub_dir);
      }
    }
    std::filesystem::remove_all(temp_dir);
    std::filesystem::create_directories(temp_dir);
    temp_dir_ = temp_dir.string();
    LOG_DEBUG() << "Created a temporary directory. Path: " << temp_dir_;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Error when creating temporary directory. Error:" << error.what();
  }
}

TempDir::~TempDir() {
  try {
    std::filesystem::remove_all(temp_dir_);
    LOG_DEBUG() << "Removed temporary directory. Path: " << temp_dir_;
  } catch (const std::exception& error) {
    LOG_ERROR() << "Failed to remove temporary directory. Error: " << error.what()
                << ", Path: " << temp_dir_;
  }
}

std::string TempDir::TempFile(const std::string &stem, const std::string &extension, bool unique_file) const {
  // Create a temporary file in my temporary directory
  try {
    auto temp_file = std::filesystem::path(temp_dir_);

    if (unique_file) {
      std::ostringstream temp;
      if (stem.empty()) {
        temp << "default_%%%%";
      } else {
        temp << stem << "_%%%%";
      }
      const auto unique = boost::filesystem::unique_path(temp.str());
      temp_file.append(unique.string());
    } else {
      if (stem.empty()) {
        temp_file.append("default");
      } else {
        temp_file.append(stem);
      }
    }
    if (extension.empty()) {
      temp_file.replace_extension(".tmp");
    } else {
      temp_file.replace_extension(extension);
    }
    return temp_file.string();
  } catch (const std::exception& error) {
    LOG_ERROR() << "Error when creating temporary file. Error:" << error.what();
  }
  return {};
}

} // end namespace