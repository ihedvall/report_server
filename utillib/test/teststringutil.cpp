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

TEST(StringUtil, FormatDoubleUnit) { //NOLINT
  std::string s;
  s = FormatDouble(11.2, 4, true, "");
  EXPECT_STREQ(s.c_str(), "11.2000");

  s = FormatDouble(11.2, 4, true, "mV");
  EXPECT_STREQ(s.c_str(), "11.2000 mV");
}

TEST(StringUtil, StringSize) { //NOLINT
  std::string data(2000, '\0');
  std::string copy(data.c_str());
  EXPECT_TRUE(copy.empty());
  EXPECT_EQ(copy.size(), 0);
}

TEST(StringUtil, WildcardMatch) { //NOLINT
  EXPECT_TRUE(WildcardMatch("OLLE", "olle", true));
  EXPECT_FALSE(WildcardMatch("OLLE", "olle", false));

  EXPECT_TRUE(WildcardMatch("OLLE", "OLLE", true));
  EXPECT_TRUE(WildcardMatch("OLLE", "OLLE", false));
  EXPECT_TRUE(WildcardMatch("OLLE", "OL", true));
  EXPECT_TRUE(WildcardMatch("OLLE", "OL", false));

  EXPECT_FALSE(WildcardMatch("OLLE", "OLLE1", false));

  EXPECT_TRUE(WildcardMatch("OLLE", "?lle", true));
  EXPECT_FALSE(WildcardMatch("OLLE", "?lle", false));

  EXPECT_TRUE(WildcardMatch("Paul was her but Olle was not", "*lle*", true));
  EXPECT_TRUE(WildcardMatch("Paul was her but Olle was not", "*lle*", false));
  EXPECT_FALSE(WildcardMatch("Paul was her but OLLE was not", "*lle*", false));
}

} //namespace util::test