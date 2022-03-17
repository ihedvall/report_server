/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "odsdocument.h"
namespace ods::gui {

class EnumPanel : public wxPanel {
 public:
  explicit EnumPanel(wxWindow* parent);

  [[nodiscard]] OdsDocument* GetDocument() const;

  void Update() override;
 private:
  wxListView* left_ = nullptr;
  wxListView* right_ = nullptr;
  wxImageList image_list_;

  void RedrawEnumList();
  void RedrawItemList();

  void OnEnumSelect(wxListEvent& event);

 wxDECLARE_EVENT_TABLE();
};

}





