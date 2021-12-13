/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include <gtest/gtest.h>
#include <util/cryptoutil.h>
namespace {
constexpr std::string_view kTestFile = "k:/test/mdf/ct/cyclelow.mf4";
}

namespace util::test {

TEST(CryptoUtil, CreateMd5FileChecksum)// NOLINT
{
  bool skip_test;
  try {
    skip_test = !std::filesystem::exists(kTestFile);
  } catch (const std::exception& ) {
    skip_test = true;
  }
  if (skip_test) {
    GTEST_SKIP();
  }

  // Test that it runs normally
  std::string md5_normal;
  const bool normal = crypto::CreateMd5FileString(kTestFile.data(), md5_normal);
  EXPECT_TRUE(normal);
  EXPECT_EQ(md5_normal.size(), 32U) << md5_normal;

  // Check that it handles files missing
  std::string md5_abnormal;
  const bool abnormal = crypto::CreateMd5FileString("testXXX.exe", md5_abnormal);
  EXPECT_FALSE(abnormal);
  EXPECT_EQ(md5_abnormal.size(), 0U);
}

} // namespace util::test
