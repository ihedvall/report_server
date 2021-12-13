/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <algorithm>
#include <boost/algorithm//string/replace.hpp>

#include "util/csvwriter.h"
#include "util/logstream.h"

namespace {
/**
 * Scan through the text and replace '"' with '""'.
 * @param [in,out] text Text string to check and maybe modify.
 */
void ConvertDittoMark(std::string& text) {
  boost::algorithm::replace_all(text, "\"", "\"\"");
}

/**
 * Scan through the text and checks if it include CRLF or any comma (','). If so
 * the text needs to be inside ditto marks ("text").
 * @param text
 */
void CheckIfDittoMarkNeeded(std::string& text) {
  auto need_ditto_mark = std::ranges::any_of(text, [] (const char in) {
    const auto byte = static_cast<unsigned char>(in);
    if (std::isspace(byte) || !std::isprint(byte) || in == ',') {
      return true;
    }
    return false;
  });

  if (need_ditto_mark) {
    std::ostringstream s;
    s << "\"" << text << "\"";
    text = s.str();
  }
}

}

namespace util::string {

CsvWriter::CsvWriter(const std::string &filename)
: file_(std::fopen(filename.c_str(), "wt")),
  filename_(filename) {
  if (file_ == nullptr) {
    util::log::LOG_ERROR() << "Failed to open the CSV file. File: " << filename_;
  }
}

CsvWriter::~CsvWriter() {
  if (file_ != nullptr) {
    std::fclose(file_);
  }
}


bool CsvWriter::IsOk() const {
  return file_ != nullptr;
}


void CsvWriter::AddColumnHeader(const std::string &header) {
  if (row_count_ != 0) {
    util::log::LOG_ERROR() << "Column headers shall be added to first row. File: " << filename_;
    return;
  }
  SaveText(header);
  max_columns_ = std::max(column_count_,max_columns_);
}

void CsvWriter::SaveText(const std::string& text) {
  if (file_ == nullptr) {
    util::log::LOG_ERROR() << "File is not open. File: " << filename_;
    return;
  }
  std::string temp = text;
  ConvertDittoMark(temp); // If temp includes a '"' it needs to be replaced by a '""'
  CheckIfDittoMarkNeeded(temp); // Check if the text needs '"' at start and end.
  if (column_count_ > 0) {
    std::fprintf(file_,",%s", temp.c_str());
  } else {
    std::fprintf(file_, "%s", temp.c_str());
  }
  ++column_count_;
}


template<>
void CsvWriter::AddColumnValue(const std::string& value) {
  if (row_count_ > 0 && column_count_ >= max_columns_) {
    AddRow();
  }
  SaveText(value);
  if (row_count_ == 0) {
    max_columns_ = std::max(column_count_, max_columns_);
  }
}

void CsvWriter::AddRow() {
  if (column_count_ == 0 && row_count_ == 0) {
    return; // No empty lines allowed
  }
  if (file_ != nullptr) {
    if (column_count_ < max_columns_) {
      for (auto ii = column_count_; ii < max_columns_; ++ii ) {
        if (ii > 0) {
          fprintf(file_, ",");
        }
      }
    }
    fprintf(file_, "\r\n");
  }
  ++row_count_;
  column_count_ = 0;
}


void CsvWriter::CloseFile() {
  if (file_ != nullptr) {
    fclose(file_);
    file_ = nullptr;
  }
}

}

