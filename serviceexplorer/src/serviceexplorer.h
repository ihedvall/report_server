/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>
#include <wx/wx.h>
#include "servicehelper.h"

namespace util::service::gui {

class ServiceExplorer : public wxApp {
 public:

  [[nodiscard]] std::string GetServiceDaemon() const {
    return serviced_;
  }
  void OpenFile(const std::string& filename) const;
  void RunAs(HWND hwnd, const std::wstring& params) const;
 protected:
  bool OnCmdLineParsed(wxCmdLineParser &parser) override;
  void OnInitCmdLine(wxCmdLineParser &parser) override;
  bool OnInit() override;
  int OnExit() override;

 private:
  wxString install_;
  wxString uninstall_;
  wxString start_;
  wxString stop_;
  std::string notepad_; ///< Path to notepad.exe
  std::string serviced_; ///< Path to serviced


  void OnOpenLogFile(wxCommandEvent& event);
  void OnUpdateOpenLogFile(wxUpdateUIEvent& event);

  void FindNotepad();
  void FindServiceDaemon();

  bool CheckCmdLine();

  wxDECLARE_EVENT_TABLE();
};

wxDECLARE_APP(ServiceExplorer);
} // namespace
