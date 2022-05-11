/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <sstream>
#include <algorithm>
#include "util/stringparser.h"
#include "util/stringutil.h"

namespace util::string {

StringParser::StringParser(const std::string &expression) {
  bool tag = false;
  std::ostringstream temp;
  for (const char cin : expression) {
    switch (cin) {
      case '<': {
        ParserElement element;
        element.is_tag = tag;
        element.name = temp.str();
        if (!element.name.empty()) {
          element_list_.emplace_back(element);
        }

        temp.str("");
        temp.clear();
        tag = true;
        break;
      }

      case '>': {
        ParserElement element;
        element.is_tag = tag;
        element.name = temp.str();
        if (!element.name.empty()) {
          element_list_.emplace_back(element);
        }

        temp.str("");
        temp.clear();
        tag = false;
        break;
      }

      default:
        temp << cin;
        break;
    }
  }
  {
    ParserElement element;
    element.is_tag = tag;
    element.name = temp.str();
    if (!element.name.empty()) {
      element_list_.emplace_back(element);
    }
  }
}

bool StringParser::Parse(const std::string &text) {
  if (element_list_.empty()) {
     return true;
  }
  for (auto& val : element_list_) {
    val.value.clear();
  }

  std::ostringstream temp;
  size_t state = 0;

  for (const char cin : text) {
    const auto& curr = element_list_[state];
    if (curr.is_tag) {
      // Check if cin is the next char
      if (state + 1 < element_list_.size()) {
        const auto& next = element_list_[state + 1];
        if (!next.name.empty() && next.name[0] == cin) {
         element_list_[state].value = temp.str();
          ++state;

          temp.str("");
          temp.clear();

          if (state >= element_list_.size()) {
            break;
          }
        }
      }
      temp << cin;
    } else {
      // Check against the name

      if (IEquals(temp.str(), curr.name)) {
        element_list_[state].value = temp.str();
        ++state;
        temp.str("");
        temp.clear();
        if (state >= element_list_.size()) {
          break;
        }
      }
      temp << cin;
    }

  }
  if (state < element_list_.size() && !temp.str().empty()) {
   element_list_[state].value = temp.str();
   ++state;
  }
  return state >= element_list_.size();
}

bool StringParser::ExistTag(const std::string &tag) const {
  return std::ranges::any_of(element_list_, [&] (const auto& element) {
    return element.is_tag && IEquals(tag, element.name);
  });
}

std::string StringParser::GetTagValue(const std::string &tag) const {
  const auto itr = std::ranges::find_if(element_list_, [&] (const auto& element) {
    return element.is_tag && IEquals(tag, element.name);
  });
  return itr == element_list_.cend() ? "" : itr->value;
}


}