/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <sstream>
#include <filesystem>
#include "childframe.h"
#include "odsconfigid.h"
#include "commonpanel.h"
#include "tablepanel.h"
#include "enumpanel.h"

namespace {

// Bitmap index for the tree control (tree_list.bmp)
constexpr int TREE_ROOT = 0;
constexpr int TREE_ID = 1;
constexpr int TREE_HD = 2;
constexpr int TREE_FH_ROOT = 3;
constexpr int TREE_FH = 4;
constexpr int TREE_DG_ROOT = 5;
constexpr int TREE_DG = 6;
constexpr int TREE_AT_ROOT = 7;
constexpr int TREE_AT = 8;
constexpr int TREE_CH_ROOT = 9;
constexpr int TREE_CH = 10;
constexpr int TREE_EV_ROOT = 11;
constexpr int TREE_EV = 12;
constexpr int TREE_CG = 13;
constexpr int TREE_SI = 14;
constexpr int TREE_CN = 15;
constexpr int TREE_CC = 16;
constexpr int TREE_CA = 17;
constexpr int TREE_DT = 18;
constexpr int TREE_SR = 19;
constexpr int TREE_RD = 20;
constexpr int TREE_SD = 21;
constexpr int TREE_DL = 22;
constexpr int TREE_DZ = 23;
constexpr int TREE_HL = 24;


} // Empty namespace

namespace ods::gui {

wxBEGIN_EVENT_TABLE(ChildFrame, wxDocMDIChildFrame) // NOLINT(cert-err58-cpp)
  EVT_UPDATE_UI(kIdSave, ChildFrame::OnUpdateSave)
  EVT_BUTTON(kIdSave, ChildFrame::OnSave)
  EVT_BUTTON(kIdSaveAs, ChildFrame::OnSaveAs)
  EVT_BUTTON(kIdClose, ChildFrame::OnCloseDoc)
  EVT_NOTEBOOK_PAGE_CHANGED(kIdNotebook, ChildFrame::OnPageChange)
wxEND_EVENT_TABLE()

ChildFrame::ChildFrame(wxDocument *doc,
                     wxView *view,
                     wxMDIParentFrame *parent,
                     wxWindowID id,
                     const wxString& title)
    : wxDocMDIChildFrame(doc, view, parent, id, title, wxDefaultPosition, wxDefaultSize,
                         wxDEFAULT_FRAME_STYLE, wxASCII_STR(wxFrameNameStr)),
                         image_list_(32,32,false,8) {
  SetIcon(wxIcon("SUB_ICON", wxBITMAP_TYPE_ICO_RESOURCE));

  image_list_.Add(wxBitmap("NOTEBOOK_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  notebook_ = new wxNotebook(this, kIdNotebook);
  notebook_->SetImageList(&image_list_);

  auto* common = new CommonPanel(notebook_);
  auto* config = new TablePanel(notebook_);
  auto* enumerate = new EnumPanel(notebook_);
  //auto* relation = new CommonPanel(notebook_);
  notebook_->AddPage(common, L"General", false,4);
  notebook_->AddPage(config, L"Database Design", false,5);
  notebook_->AddPage(enumerate, L"Enumerations", false,6);
  //notebook_->AddPage(relation, L"M:N Relations", false,7);

  auto* save_button = new wxButton(this, kIdSave, wxGetStockLabel(wxID_SAVE));
  auto* save_as_button = new wxButton(this, kIdSaveAs, wxGetStockLabel(wxID_SAVEAS));
  auto* cancel_button = new wxButton(this, kIdClose, wxGetStockLabel(wxID_CLOSE));

  auto* system_sizer = new wxBoxSizer(wxHORIZONTAL);
  system_sizer->Add(save_button, 0, wxALIGN_LEFT | wxALL, 10);
  system_sizer->Add(save_as_button, 0, wxALIGN_LEFT | wxALL, 10);
  system_sizer->Add(cancel_button, 0, wxALIGN_LEFT | wxALL, 10);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(notebook_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  save_button->SetDefault();

  notebook_->SetSelection(1);
  notebook_->SetPageImage(1,1);

}

void ChildFrame::OnUpdateSave(wxUpdateUIEvent &event) {
  const auto* doc = GetDocument();
  if (doc == nullptr) {
    event.Enable(false);
    return;
  }

  try {
    std::filesystem::path file(doc->GetFilename().ToStdString());
    if (!std::filesystem::exists(file)) {
      event.Enable(false);
      return;
    }
  } catch (const std::exception& ) {
    event.Enable(false);
    return;
  }
  event.Enable(true);
}

void ChildFrame::OnSave(wxCommandEvent&) {
  auto* doc = GetDocument();
  if (doc != nullptr) {
    doc->Save();
  }
}

void ChildFrame::OnSaveAs(wxCommandEvent&) {
  auto* doc = GetDocument();
  if (doc != nullptr) {
    doc->SaveAs();
  }
}

void ChildFrame::OnCloseDoc(wxCommandEvent& event) {
  auto *doc = GetDocument();
  if (doc != nullptr) {
    doc->DeleteAllViews();
  }
}
void ChildFrame::Update() {
  for (size_t tab = 0; tab < notebook_->GetPageCount(); ++tab) {
    auto* page = notebook_->GetPage(tab);
    if (page != nullptr) {
      page->Update();
    }
  }
  wxWindow::Update();
}
void ChildFrame::OnPageChange(wxBookCtrlEvent &event) {
  const auto old_page = event.GetOldSelection();
  const auto page = event.GetSelection();
  if (notebook_ != nullptr) {
    if (old_page != wxNOT_FOUND) {
      notebook_->SetPageImage(old_page, old_page + 4);
    }
    if (page != wxNOT_FOUND) {
      notebook_->SetPageImage(page, page);
    }
  }
}

} // end namespace



