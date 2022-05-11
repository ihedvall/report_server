/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>
namespace util::string {
class StringParser {
 public:
  explicit StringParser(const std::string &expression);
  bool Parse(const std::string& text);

  [[nodiscard]] bool ExistTag(const std::string& tag) const;
  [[nodiscard]] std::string GetTagValue(const std::string& tag) const;

 private:
  struct ParserElement {
    bool is_tag = false;
    std::string name;
    std::string value;
  };
  std::vector<ParserElement> element_list_;
};

} // end namespace




