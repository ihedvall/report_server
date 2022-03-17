/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <wx/docmdi.h>
#include <wx/valgen.h>
#include "util/timestamp.h"
#include "commonpanel.h"

namespace ods::gui {

CommonPanel::CommonPanel(wxWindow *parent) : wxPanel(parent) {
  auto* doc = GetDocument();
  if (doc == nullptr) {
    return;
  }
  const auto& model = doc->GetModel();
  name_ = model.Name();
  description_ = model.Description();
  version_ = model.Version();
  created_ = util::time::NsToLocalDate(model.Created());
  created_by_ = model.CreatedBy();
  base_version_ = model.BaseVersion();
  modified_ = util::time::NsToLocalDate(model.Modified());
  modified_by_ = model.ModifiedBy();

  auto* name = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                             wxTextValidator(wxFILTER_NONE, &name_));
  name->SetMinSize({20*10, -1});

  auto* desc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                              wxTextValidator(wxFILTER_NONE, &description_));
  desc->SetMinSize({40*10, -1});

  auto* version = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                              wxTextValidator(wxFILTER_NONE, &version_));
  version->SetMinSize({20*10, -1});

  auto* created = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                                 wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &created_));;

  auto* created_by = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                                    wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &created_by_));

  auto* base_version = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                                      wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &base_version_));

  auto* modified = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,wxDefaultSize,
                                     wxTE_READONLY | wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &modified_));

  auto* modified_by = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                                      wxTE_READONLY | wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &modified_by_));
  created_by->SetMinSize({20*10, -1});

  auto* name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto* desc_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto* version_label = new wxStaticText(this, wxID_ANY, L"Version:");
  auto* created_label = new wxStaticText(this, wxID_ANY, L"Created:");
  auto* created_by_label = new wxStaticText(this, wxID_ANY, L"Created By:");
  auto* base_version_label = new wxStaticText(this, wxID_ANY, L"Base Version:");
  auto* modified_label = new wxStaticText(this, wxID_ANY, L"Modified:");
  auto* modified_by_label = new wxStaticText(this, wxID_ANY, L"Modified By:");

  int label_width = 100;
  label_width = std::max(label_width, name_label->GetBestSize().GetX());
  label_width = std::max(label_width, desc_label->GetBestSize().GetX());
  label_width = std::max(label_width, version_label->GetBestSize().GetX());
  label_width = std::max(label_width, created_label->GetBestSize().GetX());
  label_width = std::max(label_width, created_by_label->GetBestSize().GetX());
  label_width = std::max(label_width, base_version_label->GetBestSize().GetX());
  label_width = std::max(label_width, modified_label->GetBestSize().GetX());
  label_width = std::max(label_width, modified_by_label->GetBestSize().GetX());

  name_label->SetMinSize({label_width, -1});
  desc_label->SetMinSize({label_width, -1});
  version_label->SetMinSize({label_width, -1});
  created_label->SetMinSize({label_width, -1});
  created_by_label->SetMinSize({label_width, -1});
  base_version_label->SetMinSize({label_width, -1});
  modified_label->SetMinSize({label_width, -1});
  modified_by_label->SetMinSize({label_width, -1});

  auto* name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* desc_sizer = new wxBoxSizer(wxHORIZONTAL);
  desc_sizer->Add(desc_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  desc_sizer->Add(desc, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* version_sizer = new wxBoxSizer(wxHORIZONTAL);
  version_sizer->Add(version_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  version_sizer->Add(version, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* created_sizer = new wxBoxSizer(wxHORIZONTAL);
  created_sizer->Add(created_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  created_sizer->Add(created, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* created_by_sizer = new wxBoxSizer(wxHORIZONTAL);
  created_by_sizer->Add(created_by_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  created_by_sizer->Add(created_by, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* base_version_sizer = new wxBoxSizer(wxHORIZONTAL);
  base_version_sizer->Add(base_version_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  base_version_sizer->Add(base_version, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* modified_sizer = new wxBoxSizer(wxHORIZONTAL);
  modified_sizer->Add(modified_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  modified_sizer->Add(modified, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* modified_by_sizer = new wxBoxSizer(wxHORIZONTAL);
  modified_by_sizer->Add(modified_by_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  modified_by_sizer->Add(modified_by, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* app_box = new wxStaticBoxSizer(wxVERTICAL,this, L"Application Information");
  app_box->Add(name_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM , 4);
  app_box->Add(desc_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM , 4);
  app_box->Add(version_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM , 4);

  auto* status_box = new wxStaticBoxSizer(wxVERTICAL,this, L"Model Information");
  status_box->Add(created_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM , 4);
  status_box->Add(created_by_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM , 4);
  status_box->Add(base_version_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM , 4);
  status_box->Add(modified_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM , 4);
  status_box->Add(modified_by_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM , 4);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(app_box, 0, wxALIGN_LEFT | wxALL, 4);
  main_sizer->Add(status_box, 0, wxALIGN_LEFT | wxALL, 4);

  SetSizerAndFit(main_sizer);
  CommonPanel::TransferDataToWindow();
}

OdsDocument *CommonPanel::GetDocument() const {
  const auto* child_frame = wxDynamicCast(GetGrandParent(), wxDocMDIChildFrame); // NOLINT
  return child_frame != nullptr ? wxDynamicCast(child_frame->GetDocument(), OdsDocument) : nullptr; //NOLINT
}
bool CommonPanel::TransferDataToWindow() {  auto* doc = GetDocument();
  if (doc == nullptr) {
    return false;
  }

  const auto& model = doc->GetModel();
  name_ = model.Name();
  description_ = model.Description();
  version_ = model.Version();
  created_ = util::time::NsToLocalDate(model.Created());
  created_by_ = model.CreatedBy();
  base_version_ = model.BaseVersion();
  modified_ = util::time::NsToLocalDate(model.Modified());
  modified_by_ = model.ModifiedBy();

  return wxWindowBase::TransferDataToWindow();
}
bool CommonPanel::TransferDataFromWindow() {
  return wxWindowBase::TransferDataFromWindow();
}
void CommonPanel::Update() {
  TransferDataToWindow();
  wxWindow::Update();
}

}