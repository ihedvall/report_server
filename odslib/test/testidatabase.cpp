/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <gtest/gtest.h>
#include "ods/idatabase.h"

namespace ods::test {

TEST(IDatabase, TestReservedWord) {
  EXPECT_TRUE(IsSqlReservedWord("abort"));
  EXPECT_TRUE(IsSqlReservedWord("ABORT"));
  EXPECT_FALSE(IsSqlReservedWord("aborting"));
  EXPECT_FALSE(IsSqlReservedWord("ABORTING"));
}

}

