/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <wx/docmdi.h>
#include <wx/splitter.h>
#include "enumpanel.h"
#include "odsconfigid.h"

namespace ods::gui {

wxBEGIN_EVENT_TABLE(EnumPanel, wxPanel)
  EVT_LIST_ITEM_SELECTED(kIdEnumList, EnumPanel::OnEnumSelect)
wxEND_EVENT_TABLE()

EnumPanel::EnumPanel(wxWindow *parent)
: wxPanel(parent),
  image_list_(16,16,false,2) {
  image_list_.Add(wxBitmap("ENUM_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  auto *splitter = new wxSplitterWindow(this);
  left_ = new wxListView(splitter, kIdEnumList, wxDefaultPosition, {500, 600},
                         wxLC_REPORT | wxLC_SINGLE_SEL );
  left_->AppendColumn("Name", wxLIST_FORMAT_LEFT, 200);
  left_->AppendColumn("Summary", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
  left_->SetImageList(&image_list_, wxIMAGE_LIST_SMALL);

  right_ = new wxListView(splitter, kIdItemList, wxDefaultPosition, {300, 600}, wxLC_REPORT);
  right_->AppendColumn("Item", wxLIST_FORMAT_LEFT, 50);
  right_->AppendColumn("Text", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);

  splitter->SplitVertically(left_, right_, 500);

  auto* main_sizer = new wxBoxSizer(wxHORIZONTAL);
  main_sizer->Add(splitter, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  SetSizerAndFit(main_sizer);

  RedrawEnumList();
  RedrawItemList();
}

void EnumPanel::RedrawEnumList() {
  auto *doc = GetDocument();
  if (doc == nullptr || left_ == nullptr) {
    return;
  }
  std::string selected_enum;
  auto selected = left_->GetFirstSelected();
  if (selected >= 0) {
    selected_enum = left_->GetItemText(selected, 0).ToStdString();
  }

  left_->DeleteAllItems();

  const auto &model = doc->GetModel();

  long row = 0;
  selected = -1;
  const auto& enum_list = model.Enums();
  for (const auto& itr : enum_list) {
    const IEnum& obj = itr.second;
    if (!selected_enum.empty() && obj.EnumName() == selected_enum) {
      selected = row;
    }
    std::ostringstream summary;
    const auto& item_list = obj.Items();
    for (const auto& item : item_list) {
      if (!summary.str().empty()) {
        summary << ", ";
      }
      summary << item.first <<": " << item.second;
      if (summary.str().size() > 30) {
        break;
      }
    }
    left_->InsertItem(row, obj.EnumName(), obj.Locked() ? 0 : 1);
    left_->SetItem(row, 1, wxString::FromUTF8(summary.str()));
    ++row;
  }
  if (selected >= 0) {
    left_->Select(selected);
    left_->EnsureVisible(selected);
  }
}

void EnumPanel::RedrawItemList() {
  auto *doc = GetDocument();
  if (doc == nullptr || left_ == nullptr || right_ == nullptr) {
    return;
  }

  right_->DeleteAllItems();

  const auto selected = left_->GetFirstSelected();
  if (selected < 0) {
    return;
  }

  const auto selected_enum = left_->GetItemText(selected, 0).ToStdString();
  const auto& model = doc->GetModel();
  const auto* obj = model.GetEnum(selected_enum);
  if (obj == nullptr) {
    return;
  }

  long row = 0;
  const auto& item_list = obj->Items();
  for (const auto& itr : item_list) {
    right_->InsertItem(row, std::to_string(itr.first));
    right_->SetItem(row, 1, wxString::FromUTF8(itr.second));
    ++row;
  }
}
OdsDocument *EnumPanel::GetDocument() const {
  const auto *child_frame = wxDynamicCast(GetGrandParent(), wxDocMDIChildFrame); // NOLINT
  return child_frame != nullptr ? wxDynamicCast(child_frame->GetDocument(), OdsDocument) : nullptr; //NOLINT
}

void EnumPanel::Update() {
  RedrawEnumList();
  RedrawItemList();
  wxWindow::Update();
}

void EnumPanel::OnEnumSelect(wxListEvent&) {
  RedrawItemList();
}

} // end namespace