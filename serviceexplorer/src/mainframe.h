/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>

namespace util::service::gui {
class MainFrame : public wxFrame {
 public:
  MainFrame(const wxString& title, const wxPoint& start_pos, const wxSize& start_size, bool maximized);
  void RedrawServiceList();
 private:
  wxListView* list_view_ = nullptr;
  wxCheckBox* hide_svc_ = nullptr;
  wxCheckBox* show_serviced_ = nullptr;
  wxImageList image_list_;

  void SelectItem(const wxString& name);

  void OnExit(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnFilterClick(wxCommandEvent& event);
  void OnNewService(wxCommandEvent& event);
  void OnUpdateOpenService(wxUpdateUIEvent &event);
  void OnOpenService(wxCommandEvent& event);
  void OnUpdateDeleteService(wxUpdateUIEvent &event);
  void OnDeleteService(wxCommandEvent& event);
  void OnRightClick(wxListEvent& event);
  void OnItemActivated(wxListEvent& event);
  void OnUpdateStartService(wxUpdateUIEvent &event);
  void OnStartService(wxCommandEvent& event);
  void OnUpdateStopService(wxUpdateUIEvent &event);
  void OnStopService(wxCommandEvent& event);
wxDECLARE_EVENT_TABLE();
};

} // end namespace

