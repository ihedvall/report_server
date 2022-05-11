/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <vector>
#include <array>
#include <string_view>
#include <algorithm>
#include <filesystem>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/windows.hpp>
#include "util/gnuplot.h"
#include "util/logstream.h"

using namespace util::log;

namespace {

struct TerminalDef {
  util::plot::GnuTerminal terminal;
  std::string_view script_name;
  std::string_view user_name;
};

constexpr std::array<TerminalDef, 1> TerminalNameList {
  TerminalDef{util::plot::GnuTerminal::wxt, "wxt noenhanced", "wxWidgets Terminal"},
};

std::string TerminalToScriptText(util::plot::GnuTerminal terminal) {
  const auto itr = std::ranges::find_if(TerminalNameList, [&] (const auto& term) {
    return term.terminal == terminal;
  });
  return itr == TerminalNameList.cend() ? "wxt" : itr->script_name.data();
}

} // end namespace

namespace util::plot {

GnuPlot::GnuPlot() {
  // 1. Find the path to the 'gnuplot.exe'
  try {
    std::vector< boost::filesystem::path > path_list = ::boost::this_process::path();
    path_list.emplace_back("c:/program files (x86)/gnuplot/bin");
    path_list.emplace_back("c:/program files/gnuplot/bin");

    const auto temp = boost::process::search_path("gnuplot", path_list);
    if (!temp.string().empty()) {
      exe_path_ = temp.string();
    }
    LOG_DEBUG() << "Gnuplot exe path: " << exe_path_;
  } catch(const std::exception& err) {
    LOG_ERROR() << "Failed to find gnuplot.exe. Error: " << err.what();
    exe_path_.clear();
  }
}

GnuPlot::~GnuPlot() {

}

std::string GnuPlot::CreateScript() const {
  std::ostringstream script;
  MakeScript(script);
  return script.str();
}

std::string GnuPlot::FileName() const {
  // The file name is created from the CSV filename. So it is critical that
  // the CSV file name is set before calling this function
  std::string filename;
  try {
    std::filesystem::path temp(data_file_.FullName());
    temp.replace_extension(".gp");
    filename = temp.string();
  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to create the gnuplot filename. Error: " << err.what()
                << ", CSV Name: " << data_file_.FullName();
  }
  return filename;
}

std::string GnuPlot::CsvFileName() const {
  std::string filename;
  try {
    std::filesystem::path temp(Path());
    temp.append(Name());
    temp.replace_extension(".csv");
    filename = temp.string();
  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to create the CSV filename. Error: " << err.what()
                << ", Path: " << Path() << ", Name:" << Name();
  }
  return filename;
}

void GnuPlot::SaveScript() {
  const auto filename = FileName();
  try {
    std::ofstream file;
    file.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    MakeScript(file);
    file.close();
  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to save the gnuplot file. File: " << filename;
  }
}

void GnuPlot::MakeScript(std::ostream& script) const {
  std::string unit_y1;
  std::string unit_y2;
  const size_t nof_cols = data_file_.Columns();
  size_t y2_column = nof_cols;


  // Note column 0 is always the X-axis
  for (size_t column = 1; column < nof_cols; ++column) {
     const auto unit = data_file_.Unit(column);
    if (column == 1) {
      unit_y1 = unit;
    } else if (unit != unit_y1 && y2_column == nof_cols) {
      unit_y2 = unit;
      y2_column = column;
    }
  }
  script << "set terminal " << TerminalToScriptText(terminal_) << " size " << x_size_ << "," << y_size_ << std::endl;
  script << "set datafile separator comma columnhead" << std::endl;
  script << "set key outside" << std::endl;
  script << "set grid" << std::endl;
  script << "set style data lines" << std::endl;
  script << "set autoscale" << std::endl;

  // Some weird gnuplot feature for displaying y2 axis correctly
  if (y2_column < nof_cols) {
    script << "set tics nomirror" << std::endl;
    script << "set y2tics" << std::endl;
  }

  script << "set title \"" << title_ << "\"" << std::endl;
  script << "set xlabel \"" << data_file_.Label(0) << "\"" << std::endl;
  script << "set ylabel \"" << data_file_.Unit(1) << "\"" << std::endl;

  if (y2_column < nof_cols) {
    script << "set y2label \"" << data_file_.Unit(y2_column) << "\"" << std::endl;
  }

  for (size_t column = 1; column < nof_cols; ++column) {
    const auto unit = data_file_.Unit(column);
    if (y2_column < nof_cols && column > y2_column && unit != unit_y2) {
      continue; // Cannot plot this value
    }
    if (column == 1) {
      script << "plot '" << data_file_.FileName() << "' using 1:2 axis x1y1";
    } else if (column < y2_column) {
      script << "'' using 1:" << column + 1 << " axis x1y1";
    } else {
      script << "'' using 1:" << column + 1 << " axis x1y2";
    }
    script << " title \"" << data_file_.Label(column) << "\"";
    if (column + 1 < nof_cols) {
      script << ",\\";
    }
    script << std::endl;
  }
  script << "exit" << std::endl;
}

void GnuPlot::Show() {
  try {
    std::filesystem::path temp(FileName());
    // Need to restore the current directory otherwise is the temporary directory not deleted
    const auto script_dir = temp.parent_path();
    const auto curr_dir = std::filesystem::current_path();
    std::filesystem::current_path(script_dir);
    boost::process::spawn(exe_path_, "--persist", FileName(), boost::process::windows::hide);
    std::filesystem::current_path(curr_dir);
  } catch (const std::exception& err) {
    LOG_ERROR() << "Failed to run gnuplot directory. Error: " << err.what();
  }


}

} // end namespace
