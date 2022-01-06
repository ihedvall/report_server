/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <gtest/gtest.h>
#include "util/ilisten.h"

namespace util::test {

 class ListenMock : public util::log::IListen {
 public:
  ListenMock() = default;
  ~ListenMock() override;
  [[nodiscard]] bool IsActive() const override;
   [[nodiscard]] size_t LogLevel() override;

  protected:
  void AddMessage(uint64_t nano_sec_1970, const std::string &pre_text, const std::string &text) override;
};

class TestListen: public ::testing::Test {
 protected:
  static void SetUpTestCase();
  static void TearDownTestCase();
};

} // end namespace util::test




