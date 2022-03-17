/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/wx.h>
#include "odsdocument.h"

namespace ods::gui {

class CommonPanel : public wxPanel {
 public:

  explicit CommonPanel(wxWindow* parent);

  [[nodiscard]] OdsDocument* GetDocument() const;
  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
  void Update() override;
 private:
  wxString name_;
  wxString description_;
  wxString version_;
  wxString created_;
  wxString created_by_;
  wxString base_version_;
  wxString modified_;
  wxString modified_by_;
};

} // end namespace




