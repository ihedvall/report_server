/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <gtest/gtest.h>
#include <util/stringutil.h>
using namespace util::string;

namespace util::test {

TEST(StringUtil, FormatDoubleNotFixed) {
  std::string s;
  s = FormatDouble(11.23456, 0);
  EXPECT_STREQ(s.c_str(), "11");

  s = FormatDouble(11.56, 0);
  EXPECT_STREQ(s.c_str(), "12");

  s = FormatDouble(1.23456, 1);
  EXPECT_STREQ(s.c_str(), "1.2");

  s = FormatDouble(1.23456, 3);
  EXPECT_STREQ(s.c_str(), "1.235");

  s = FormatDouble(1.23456, 6);
  EXPECT_STREQ(s.c_str(), "1.23456");

  s = FormatDouble(-1.23456, 2);
  EXPECT_STREQ(s.c_str(), "-1.23");

  s = FormatDouble(22.000001, 2);
  EXPECT_STREQ(s.c_str(), "22");

  s = FormatDouble(2200000012.34, 2);
  EXPECT_STREQ(s.c_str(), "2.2E+09");

  s = FormatDouble(-2200000012.34, 2);
  EXPECT_STREQ(s.c_str(), "-2.2E+09");

  s = FormatDouble(0.0056, 3);
  EXPECT_STREQ(s.c_str(), "0.006");

  s = FormatDouble(-0.0050000001, 3);
  EXPECT_STREQ(s.c_str(), "-0.005");

  s = FormatDouble(2200000000.00, 2);
  EXPECT_STREQ(s.c_str(), "2.2E+09");
}

TEST(StringUtil, FormatDoubleFixed) {
  std::string s;
  s = FormatDouble(11.23456, 0, true);
  EXPECT_STREQ(s.c_str(), "11");

  s = FormatDouble(11.56, 0, true);
  EXPECT_STREQ(s.c_str(), "12");

  s = FormatDouble(1.23456, 1, true);
  EXPECT_STREQ(s.c_str(), "1.2");

  s = FormatDouble(1.23456, 3, true);
  EXPECT_STREQ(s.c_str(), "1.235");

  s = FormatDouble(1.23456, 6, true);
  EXPECT_STREQ(s.c_str(), "1.234560");

  s = FormatDouble(-1.23456, 2, true);
  EXPECT_STREQ(s.c_str(), "-1.23");

  s = FormatDouble(22.000001, 2, true);
  EXPECT_STREQ(s.c_str(), "22.00");

  s = FormatDouble(2200000012.34, 2, true);
  EXPECT_STREQ(s.c_str(), "2.2E+09");

  s = FormatDouble(-2200000012.34, 2, true);
  EXPECT_STREQ(s.c_str(), "-2.2E+09");

  s = FormatDouble(0.0056, 3, true);
  EXPECT_STREQ(s.c_str(), "0.006");

  s = FormatDouble(-0.0050000001, 3, true);
  EXPECT_STREQ(s.c_str(), "-0.005");

  s = FormatDouble(2200000000.00, 2, true);
  EXPECT_STREQ(s.c_str(), "2.2E+09");
}

TEST(StringUtil, FormatDoubleUnit) {
  std::string s;
  s = FormatDouble(11.2, 4, true, "");
  EXPECT_STREQ(s.c_str(), "11.2000");

  s = FormatDouble(11.2, 4, true, "mV");
  EXPECT_STREQ(s.c_str(), "11.2000 mV");
}

} //namespace util::test