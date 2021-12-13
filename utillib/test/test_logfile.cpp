/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <iostream>
#include <filesystem>
#include <gtest/gtest.h>
#include "util/logconfig.h"
#include "../src/logfile.h"

using namespace util::log;
using namespace util::log::detail;
using namespace std::filesystem;

namespace util::test {
TEST(LogFile, TestFileSizeNotExist) {
  try {
    path not_exists("c:/donald.duck");
    if (file_size(not_exists) > 10) {
      FAIL() << "Not expected";
    }
  } catch (const std::exception& err) {
    SUCCEED() << err.what();
  } catch(...) {
    FAIL() << "Unknown error";
  }
}

TEST(LogFile, OpenCloseWithExtension)// NOLINT
{
  try {
    // Test normal usage
    auto &log_config = LogConfig::Instance();
    log_config.BaseName("olle.log");
    log_config.SubDir("Testing");
    log_config.Type(LogType::LogToFile);
    log_config.CreateDefaultLogger();

    auto *logger = dynamic_cast<const LogFile *>(log_config.GetLogger("Default"));
    ASSERT_TRUE(logger != nullptr);

    const std::string full_path = logger->Filename();
    log_config.DeleteLogChain();
    std::cout << full_path << std::endl;
    EXPECT_FALSE(full_path.empty()) << full_path << std::endl;
  } catch (const std::exception &error) {
    GTEST_FAIL() << error.what() << std::endl;
  }
}

TEST(LogFile, OpenCloseWithoutExtension)// NOLINT
{
  try {
    // Test normal usage
    auto &log_config = LogConfig::Instance();
    log_config.BaseName("pelle.log");
    log_config.SubDir("Testing");
    log_config.Type(LogType::LogToFile);
    log_config.CreateDefaultLogger();

    auto *logger = dynamic_cast<const LogFile *>(log_config.GetLogger("Default"));
    ASSERT_TRUE(logger != nullptr);

    std::string full_path = logger->Filename();
    log_config.DeleteLogChain();
    std::cout << full_path << std::endl;
    EXPECT_FALSE(full_path.empty()) << full_path << std::endl;
  } catch (const std::exception &error) {
    GTEST_FAIL() << error.what() << std::endl;
  }
}

TEST(LogFile, OpenCloseFullPath)// NOLINT
{
  try {
    // Test normal usage
    auto &log_config = LogConfig::Instance();
    path curr = current_path();
    curr.replace_filename("test.log");
    log_config.BaseName(curr.string());
    log_config.SubDir("Testing");
    log_config.Type(LogType::LogToFile);
    log_config.CreateDefaultLogger();

    auto *logger = dynamic_cast<const LogFile *>(log_config.GetLogger("Default"));
    ASSERT_TRUE(logger != nullptr);

    std::string full_path = logger->Filename();
    log_config.DeleteLogChain();
    std::cout << full_path << std::endl;
    const path temp_path(full_path);
    EXPECT_TRUE(temp_path.has_parent_path());
    EXPECT_TRUE(temp_path.parent_path() == curr.parent_path())
              << curr << "->" << temp_path << std::endl;
    EXPECT_FALSE(full_path.empty()) << full_path << std::endl;
  } catch (const std::exception &error) {
    GTEST_FAIL() << error.what() << std::endl;
  }
}

} // namespace util::test

