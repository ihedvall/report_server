/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>
#include <wx/wx.h>


namespace util::log::gui {

class ListenViewer : public wxApp {
 public:
  void OpenFile(const std::string& filename) const;

 protected:
  bool OnInit() override;
  int OnExit() override;

 private:
  std::string notepad_; ///< Path to notepad.exe



  void OnOpenLogFile(wxCommandEvent& event);
  void OnUpdateOpenLogFile(wxUpdateUIEvent& event);

  void FindNotepad();


  bool CheckCmdLine();

  wxDECLARE_EVENT_TABLE();
};

wxDECLARE_APP(ListenViewer);
} // namespace
