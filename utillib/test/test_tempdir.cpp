/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <string_view>
#include <filesystem>
#include <gtest/gtest.h>
#include "util/tempdir.h"
#include "util/stringutil.h"

using namespace util::log;
using namespace util::string;

namespace {
  constexpr std::string_view kDonaldDir = "donald";
  constexpr std::string_view kDuckFile = "duck";

}



namespace util::test {

TEST(TempDir, NormalFunction ) {
  std::string path;
  {
    TempDir test_dir(kDonaldDir.data(), false);
    path = test_dir.Path();
    std::cout << path << std::endl;
    EXPECT_FALSE(path.empty());
    EXPECT_TRUE(std::filesystem::exists(path));
  }
  EXPECT_FALSE(std::filesystem::exists(path));
}

TEST(TempDir, UniqueFunction ) {
  std::string path;
  for (size_t count = 0; count < 100; ++count) {
    {
      TempDir test_dir(kDonaldDir.data(), true);
      path = test_dir.Path();
      std::cout << path << std::endl;
      EXPECT_FALSE(path.empty());
      EXPECT_TRUE(std::filesystem::exists(path));
    }
    EXPECT_FALSE(std::filesystem::exists(path));
  }
}

TEST(TempDir, DefaultFunction ) {
  std::string path;
  {
    TempDir test_dir("", false);
    path = test_dir.Path();
    std::cout << path << std::endl;
    EXPECT_FALSE(path.empty());
    EXPECT_TRUE(std::filesystem::exists(path));
  }
  EXPECT_FALSE(std::filesystem::exists(path));
}

TEST(TempDir, NormalTempFile ) {
  TempDir test_dir(kDonaldDir.data(), false);
  const auto file = test_dir.TempFile(kDuckFile.data(), ".xxx", false);
  std::cout << file << std::endl;
  EXPECT_FALSE(file.empty());
}

TEST(TempDir, UniqueTempFile ) {
  TempDir test_dir(kDonaldDir.data(), false);
  const auto file = test_dir.TempFile(kDuckFile.data(), ".xxx", true);
  std::cout << file << std::endl;
  EXPECT_FALSE(file.empty());
}

TEST(TempDir, BadFileExtension) {
  TempDir test_dir(kDonaldDir.data(), false);
  const auto file = test_dir.TempFile(kDuckFile.data(), "yyy", true);
  std::cout << file << std::endl;
  EXPECT_FALSE(file.empty());
}
}