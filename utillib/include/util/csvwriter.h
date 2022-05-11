/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file csvwriter.h
 * \brief Defines a simple API when creating a comma separated value (CSV) file.
 */
#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <utility>
#include <vector>

namespace util::plot {

/** \class CsvWriter csvwriter.h "util/csvwriter.h"
 * \brief Simple writer interface when creating a CSV file (comma separated value).
 *
 * The class simplifies writing of CSV files. It fixes all tricks about floating point
 * dots which are comma in some countries. The first row in a CSV file may be the values
 * name wnd unit, typically formatted as 'name [unit]'.
 *
 * Usage:
 * \code {.cpp}
 * CsvWriter csv_file(filename); // Opens the file. The filename shall have full path.
 * csv_file.AddColumnHeader("Donald","duck");
 * csv_file.AddColumnHeader("Micky, "mouse");
 * csv_file.AddRow();
 * csv_file.AddColumnValue(value1);
 * csv_file.AddColumnValue(value2);
 * csv_file.CloseFile(); // Closes the file. Running the destructor will do the same.
 * \endcode
 */
class CsvWriter {
 public:


/** \brief Constructor that opens a CSV file.
 *
 * This constructor opens the CSV file. Note that the destructor closes the file.
 * @param filename File name with full path and extension.
 */
  explicit CsvWriter(const std::string& filename);

/** \brief Constructor that opens a string stream instead of a physical file .
 *
 * This constructor uses an internal string stream instead of a physical file.
 *
 */
  CsvWriter();
/** \brief  Destructor that closes the CSV file.
 */
  virtual ~CsvWriter();


  /**
   * Closes the CSV file.
   */
  void CloseFile();

  /**
   * Returns the file name without path but with it's extension.
   * @return Returns the file name without path.
   */
  [[nodiscard]] std::string FileName() const;

  /**
   * Returns the file name with full path
   * @return The file name
   */
  [[nodiscard]] const std::string& FullName() const {
    return filename_;
  }


  /**
   * Opens a file for writing. Any existing file will be truncated
   * @param filename Full path file name of the CSV file
   */
  void OpenFile(const std::string& filename);

/** \brief Returns true if the file is successfully opened.
 *
 * Checks if the file is open and if so return true.
 * @return True if file is open.
 */
  [[nodiscard]] bool IsOk() const;

/** \brief Adds a column header string.
 *
 * Adds a header string to the first row in the file. Normally is the header text 'name [unit]'.
 * @param header The header text.
 */
  void AddColumnHeader(const std::string& name, const std::string& unit, bool valid = true);
  void SetColumnValid(size_t column, bool valid = true);
  [[nodiscard]] bool IsColumnValid(size_t column) const;

/** \brief Starts a new row.
 * S
 */
  void AddRow();

/** \brief Adds a column value.
 * Adds a column value to the file.
 * @tparam T Type of value
 * @param value The value to save to the file.
 */
  template <typename T>
  void AddColumnValue(const T& value);

  [[nodiscard]] std::string GetText() const {
    return text_.str();
  }

  void Reset();

  [[nodiscard]] size_t Columns() const {
    return max_columns_;
  }

  [[nodiscard]] size_t Rows() const {
    return row_count_;
  }

  [[nodiscard]] const std::string Label(size_t index) const;
  [[nodiscard]] const std::string& Name(size_t index) const;
  [[nodiscard]] const std::string& Unit(size_t index) const;
 private:
  std::ofstream file_;
  std::ostringstream text_;
  std::ostream* output_ = nullptr;

  std::string filename_; ///< Full path filename. Mainly used for error logs.
  size_t column_count_ = 0; ///< Internal message_semaphore that check that correct number of values in each row
  size_t max_columns_ = 0; ///< Is set in first row to number of columns in the file
  size_t row_count_ = 0; ///< Keeps track of number of rows.

  struct Header {
    std::string name;   ///< Value name
    std::string unit;   ///< Value Unit
    bool valid = false; ///< Set to true if any value is valid
  };
  using NameUnitList = std::vector<Header>;
  NameUnitList header_list_;

  /** \brief Support function that save a text string to the file.
  * Support function that saves a text value.
  * @param text Text to be saved.
  */
  void SaveText(const std::string& text);
};


template<typename T>
void CsvWriter::AddColumnValue(const T &value) {
  auto temp = std::to_string(value);
  SaveText(temp);
  if (row_count_ == 0) {
    max_columns_ = std::max(column_count_, max_columns_);
  }
}


/** \brief Adds a column text string.
 *
 * Function that adds a string column value to file.
 * @param value Text to save.
 */
template <>
void CsvWriter::AddColumnValue<std::string>(const std::string& value);

template <>
void CsvWriter::AddColumnValue<float>(const float& value);

template <>
void CsvWriter::AddColumnValue<double>(const double& value);

template <>
void CsvWriter::AddColumnValue<bool>(const bool& value);
} // end namespace util::string
