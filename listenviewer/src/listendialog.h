/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/listctrl.h>

namespace util::log::gui {

struct ListenSelect {
  bool active = false;
  std::wstring name;
  std::wstring host = L"127.0.0.1";
  std::wstring description;
  uint16_t port = 0;
};

using SelectionList = std::vector<ListenSelect>;

class ListenDialog : public wxDialog {
 public:
  SelectionList selection_list_;
  wxString remote_name_;
  wxString remote_host_ = L"127.0.0.1";
  wxString remote_description_;
  uint16_t remote_port_ = 0;

  explicit ListenDialog(wxWindow *parent);
  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:
  wxListView* list_ = nullptr;
  wxImageList image_list_;
  void OnActivateChange(wxListEvent& event);
  void OnRightClick(wxListEvent& event);
  void OnEnable(wxCommandEvent& event);
  void OnDisable(wxCommandEvent& event);
 wxDECLARE_EVENT_TABLE();
};
} // end namespace



