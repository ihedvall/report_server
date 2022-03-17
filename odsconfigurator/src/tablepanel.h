/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/treelist.h>
#include "odsdocument.h"
namespace ods::gui {

class TablePanel : public wxPanel {
 public:
  explicit TablePanel(wxWindow* parent);

  [[nodiscard]] OdsDocument* GetDocument() const;

  void Update() override;
 private:
  wxTreeListCtrl* left_ = nullptr;
  wxListView* right_ = nullptr;
  wxImageList image_list_;
  wxStaticText* table_info_ = nullptr;

  void RedrawTableList();
  void RedrawColumnList();

  void OnTableSelect(wxTreeListEvent& event);

 wxDECLARE_EVENT_TABLE();
};

}




