/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file csvwriter.h
 * \brief Defines a simple API when creating a comma separated value (CSV) file.
 */
#pragma once
#include <string>
#include <sstream>
#include <cstdio>

namespace util::string {

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
 * csv_file.AddColumnHeader("Donald [duck]");
 * csv_file.AddColumnHeader("Micky [mouse]");
 * csv_file.AddRow();
 * csv_file.AddColumnValue(value1);
 * csv_file.AddColumnValue(value2);
 * csv_file.CloseFile(); // Closes the file. Running the destructor will do the same.
 * \endcode
 */
class CsvWriter final {
 public:
/** \brief Constructor that opens the CSV file.
 *
 * This constructor opens the CSV file. Note that the destructor closes the file.
 * @param filename File name with full path and extension.
 */
  explicit CsvWriter(const std::string& filename);

/** \brief  Destructor that closes the CSV file.
 */
  ~CsvWriter();

  CsvWriter() = delete;
/** \brief Closes the CSV file.
 */
  void CloseFile();

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
  void AddColumnHeader(const std::string& header);

/** \brief Starts a new row.
 * S
 */
  void AddRow();

/** \brief Adds a column value.
 * Adds a column value to the file. Note that only numbers and string are supported.
 * @tparam T Type of value
 * @param value The value to save to the file.
 */
  template <typename T>
  void AddColumnValue(const T& value); ///<

/** \brief Adds a column text string.
 *
 * Function that adds a string column value to file.
 * @param value Text to save.
 */
  template <typename T = std::string>
  void AddColumnValue(const std::string& value);

 private:
  std::FILE* file_ = nullptr; ///< Note that the file is opened by the constructor and closed by the destructor
  std::string filename_; ///< Full path filename. Mainly used for error logs.
  size_t column_count_ = 0; ///< Internal counter that check that correct number of values in each row
  size_t max_columns_ = 0; ///< Is set in first row to number of columns in the file
  size_t row_count_ = 0; ///< Keeps track of number of rows.

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

} // end namespace util::string
