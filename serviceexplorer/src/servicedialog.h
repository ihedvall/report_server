/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>
#include <wx/filepicker.h>
#include "serviceinfo.h"
namespace util::service::gui {

class ServiceDialog : public wxDialog {
 public:
  ServiceDialog(wxWindow *parent, wxWindowID ident, const wxString &title);
  void SetInfo(const ServiceInfo& info, bool new_info);
  const ServiceInfo& GetInfo() const;
  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:
  ServiceInfo info_;
  bool new_info_ = true;
  wxString args_;
  wxString exe_args_;

  wxButton* save_button_ = nullptr;
  wxButton* apply_button_ = nullptr;
  wxButton* cancel_button_ = nullptr;
  wxFilePickerCtrl* app_picker_ = nullptr;
  wxFilePickerCtrl* exe_picker_ = nullptr;
  wxTextCtrl* name_ = nullptr;
  wxTextCtrl* display_name_ = nullptr;
  wxTextCtrl* description_ = nullptr;
  wxTextCtrl* app_name_ = nullptr;
  wxTextCtrl* app_path_ = nullptr;
  wxTextCtrl* arguments_ = nullptr;
  wxTextCtrl* dependency_ = nullptr;
  wxTextCtrl* exe_arguments_ = nullptr;
  wxChoice* startup_ = nullptr;
  wxChoice* priority_ = nullptr;

  void OnUpdateApply(wxUpdateUIEvent &event);
  void OnApply(wxCommandEvent& event);
  void OnUpdateSave(wxUpdateUIEvent &event);
  void OnSave(wxCommandEvent& event);
  void OnAppPicker(wxFileDirPickerEvent& event);
  void OnNameChange(wxCommandEvent& event);
  void OnExePicker(wxFileDirPickerEvent& event);
  void DoInstall();
 wxDECLARE_EVENT_TABLE();
};

} // end namespace


