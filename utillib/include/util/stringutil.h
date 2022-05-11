/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>

namespace util::string
{
  bool IEquals(const std::string &s1, const std::string &s2, size_t nChar = 0);
  bool IEquals(const std::wstring &s1, const std::wstring &s2, size_t nChar = 0);
  /// This class is used when get a sorted list of string to ignoring case characters.\n
  /// Example of declaring a map:
  /// \code
  /// std::map<std::string, FooClass, util::string::IgnoreCase> foo_list;
  /// \endcode
  class IgnoreCase final
  {
  public:
    bool operator()(const std::string &s1, const std::string &s2) const; ///< Compare the strings by ignoring case.
    bool operator()(const std::wstring &s1, const std::wstring &s2) const; ///< Compare the strings by ignoring case.
  };

  void Trim(std::string &text);
  [[nodiscard]] std::string Trim(const std::string &text);

/// Converts a floating point value to a string using number of decimals.\n
/// It also fix rounding and returning a fixed decimals.
/// Presenting fixed number of decimals means that it fills up the string with '0' characters.\n
/// Example: Value: 1.23 and decimals 3,String: (Fixed = false) "1.23" (Fixed = true) "1.230"\n
/// Optional it can append a unit to the string (Example: "1.23 m/s").\n
/// \param[in] value  The floating point value.
/// \param[in] decimals Max number of decimals.
/// \param[in] fixed If it should show fixed number of decimals.
/// \param[in] unit Appends a unit string to the output.
/// \return The formatted string.
  std::string FormatDouble(double value, uint8_t decimals, bool fixed = false,
                           const std::string &unit = {}); ///< Converts a float to a string.

/**
 * Compare a string against a simple * or ? wildcard string
 * @param text The text to match
 * @param wildcard The wildcard string
 * @param ignore_case True if the algorithm should ignore case
 * @return Returns True if the text string match the wildcard pattern
 */
bool WildcardMatch(const std::string& text, const std::string& wildcard, bool ignore_case );

/** \brief Converts a float to string without loosing precision.
 *
 * Converts a float value to a string without loosing any precision.
 * @param value Float value to convert
 * @return The float value as text
 */
[[nodiscard]] std::string FloatToString(float value);

/** \brief Converts a double to string without loosing precision.
 *
 * Converts a double value to a string without loosing any precision.
 * @param value Double value to convert
 * @return The double value as text
 */
[[nodiscard]] std::string DoubleToString(double value);
} //namespace util::string
