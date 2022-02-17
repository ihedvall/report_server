/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <cstring>
#include <cmath>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "util/stringutil.h"


namespace util::string {
bool IEquals(const std::string &s1, const std::string &s2, size_t nChar) {
  return nChar == 0 ? stricmp(s1.c_str(), s2.c_str()) == 0 :
         strnicmp(s1.c_str(), s2.c_str(), nChar) == 0;
}

bool IEquals(const std::wstring &s1, const std::wstring &s2, size_t nChar) {
  return nChar == 0 ? wcsicmp(s1.c_str(), s2.c_str()) == 0 :
         wcsnicmp(s1.c_str(), s2.c_str(), nChar) == 0;
}

bool IgnoreCase::operator()(const std::string &s1, const std::string &s2) const {
  return boost::algorithm::ilexicographical_compare(s1,s2);
}

bool IgnoreCase::operator()(const std::wstring &s1, const std::wstring &s2) const {
  return boost::algorithm::ilexicographical_compare(s1,s2);
}

void Trim(std::string &text) {
  boost::algorithm::trim(text);
}

std::string FormatDouble(double value, uint8_t decimals, bool fixed, const std::string& unit) {
  // Maximize it to 20 decimals
  if (decimals > 20) {
    decimals = 20;
  }

  // If no decimals, the fixed must be false
  if (decimals == 0) {
    fixed = false;
  }

  // Find type of decimal point character
  const auto loc = std::locale("");
  const auto& facet = std::use_facet<std::numpunct<char>>(loc);
  const char dec_char = facet.decimal_point();

  std::string text;
  auto value_int =  static_cast<int64_t> (value);
  if (value == value_int && !fixed) {
    // If the value actually is an integer then just show it as an integer
    text = std::to_string(value_int);
  } else if (decimals == 0) {
    // Fixed round of float
    value_int = static_cast<int64_t>(std::floor(value + 0.5));
    text = std::to_string(value_int);
  } else {
    char format[20] {};
    sprintf(format, "%%%c%df", dec_char, static_cast<int>(decimals));
    char temp[200] {};
    sprintf(temp,format, value);
    text = temp;
  }

  // If the value is producing to many digits, then convert it to a string with exponent
  const auto size = text.size();
  if (size > static_cast<size_t>(6 + decimals) ) {
    char temp[200] {};
    sprintf(temp, "%G", value);
    text = temp;
  }

  // fill or delete trailing '0' but not if using exponent
  const bool have_decimal = text.find(dec_char) != std::string::npos && text.find('E') == std::string::npos;
  if (!fixed && have_decimal) {
    // Remove trailing zeros
    // Check that it is a decimal point in the string
    while (text.back() == '0'){
      text.pop_back();
    }
  } else if (fixed && have_decimal) {
    const size_t dec_pos = text.find(dec_char);
    const std::string dec = text.substr(dec_pos + 1);
    for (size_t nof_dec = dec.size(); nof_dec < decimals; ++nof_dec) {
      text.push_back('0');
    }
  }

  // We don't want to display '22.' instead of 22
  if (text.back() == dec_char) {
    text.pop_back();
  }

  if (!unit.empty()) {
    text += " ";
    text += unit;
  }
  return text;
}

}

