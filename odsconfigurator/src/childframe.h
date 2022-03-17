/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/wx.h>
#include <wx/docmdi.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>

namespace ods::gui {
class ChildFrame : public wxDocMDIChildFrame {
 public:
  ChildFrame(wxDocument *doc,
            wxView *view,
            wxMDIParentFrame *parent,
            wxWindowID id,
            const wxString& title);
  ChildFrame() = default;
  void Update() override;

 private:
  wxImageList image_list_;
  wxNotebook* notebook_ = nullptr;
  void OnUpdateSave(wxUpdateUIEvent &event);
  void OnSave(wxCommandEvent&);
  void OnSaveAs(wxCommandEvent&);
  void OnCloseDoc(wxCommandEvent&);
  void OnPageChange(wxBookCtrlEvent& event);
  wxDECLARE_EVENT_TABLE();

};
}




