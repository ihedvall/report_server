/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include "ods/imodel.h"
#include <ods/baseattribute.h>
namespace ods::gui {

class TableDialog : public wxDialog {
 public:
  TableDialog(wxWindow* parent, const IModel& model, const ITable* original );
  [[nodiscard]] const ITable& GetTable() const {
    return table_;
  }
  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:
  ITable original_; ///< The original table
  ITable table_; ///< The modified table
  const IModel& model_; ///< Reference to the model
  wxListView* base_list_ = nullptr;
  wxImageList image_list_;

  int64_t application_id_ = 0;
  wxString application_name_;
  wxString description_;
  wxString base_name_;
  wxString parent_;
  wxString database_name_;
//  int64_t security_mode_ = 0;
  std::vector<BaseAttribute> attr_list_;
  void FetchBaseAttribute();
  void RedrawBaseList();
  void OnToggleSelect(wxListEvent& event);
  void OnBaseChange(wxCommandEvent& event);
  void OnParentChange(wxCommandEvent& event);
 wxDECLARE_EVENT_TABLE();
};

}




