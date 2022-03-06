/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>

namespace util::log::gui {

class SettingDialog : public wxDialog {
 public:
  size_t max_lines_ = 10000;
  size_t wrap_around_ = 120;
  bool show_us_ = false;
  bool show_date_ = false;

  explicit SettingDialog(wxWindow *parent);

 private:
  wxDECLARE_EVENT_TABLE();
};

} // end namespace



