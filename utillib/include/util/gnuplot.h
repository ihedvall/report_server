/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file gnuplot.h
 * \brief Simple wrapper to simplify plotting using the gnuplot application.
 */
#pragma once
#include <string>
#include "util/csvwriter.h"

namespace util::plot {

enum class GnuTerminal {
  wxt = 0
};

class GnuPlot {
 public:
  GnuPlot();
  virtual ~GnuPlot();

  [[nodiscard]] const std::string& ExePath() const {
    return exe_path_;
  }

  [[nodiscard]] const std::string& Name() const {
    return name_;
  }
  void Name(const std::string& name) {
    name_ = name;
  }

  [[nodiscard]] const std::string& Path() const {
    return path_;
  }
  void Path(const std::string& path) {
    path_ = path;
  }
  std::string FileName() const;
  std::string CsvFileName() const;
  std::string CreateScript() const;
  void SaveScript();
  void Show();

  CsvWriter& CsvFile() {
    return data_file_;
  }

 private:
  std::string exe_path_; ///< Full path to the gnuplot executable.
  std::string name_; ///< Name on CSV and gnuplot files, No path or extension.
  std::string path_; ///< Directory where the gnuplot and its data files are stored.

  GnuTerminal terminal_ = GnuTerminal::wxt;

  std::string title_;
  size_t x_size_ = 1200;
  size_t y_size_ = 800;

  std::string x_label_;
  std::string y_label_;
  std::string x2_label_;
  std::string y2_label_;

  CsvWriter data_file_;

  void MakeScript(std::ostream& script) const;
};

}