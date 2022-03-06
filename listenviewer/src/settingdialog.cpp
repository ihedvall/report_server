/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <wx/config.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>
#include <wx/valgen.h>
#include "settingdialog.h"

namespace util::log::gui {

wxBEGIN_EVENT_TABLE(SettingDialog, wxDialog) //NOLINT

wxEND_EVENT_TABLE()

SettingDialog::SettingDialog(wxWindow *parent)
    : wxDialog(parent,wxID_ANY,L"General Settings", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ) {

  auto* save_button_ = new wxButton(this, wxID_OK, wxGetStockLabel(wxID_SAVE));
  auto* cancel_button_ = new wxButton(this, wxID_CANCEL, wxGetStockLabel(wxID_CANCEL));

  auto max_lines = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                                  wxIntegerValidator(&max_lines_,wxNUM_VAL_THOUSANDS_SEPARATOR));
  max_lines->SetMaxLength(10);
  max_lines->SetMinSize({10*10,-1});

  auto wrap_around = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                                  wxIntegerValidator(&wrap_around_));
  wrap_around->SetMaxLength(3);
  wrap_around->SetMinSize({3*10,-1});

  auto show_us = new wxCheckBox(this, wxID_ANY, L"Show Microseconds",wxDefaultPosition, wxDefaultSize,0,
      wxGenericValidator(&show_us_));

  auto show_date = new wxCheckBox(this, wxID_ANY, L"Show Date",wxDefaultPosition, wxDefaultSize,0,
                                wxGenericValidator(&show_date_));


  auto* max_lines_label = new wxStaticText(this, wxID_ANY, L"Max Number of Lines:");
  auto* wrap_around_label = new wxStaticText(this, wxID_ANY, L"Wrap Around:");

  int label_width = 100;
  label_width = std::max(label_width,max_lines_label->GetBestSize().GetX());
  label_width = std::max(label_width, wrap_around_label->GetBestSize().GetX());

  max_lines_label->SetMinSize({label_width, -1});
  wrap_around_label->SetMinSize({label_width, -1});

  auto* max_line_sizer = new wxBoxSizer(wxHORIZONTAL);
  max_line_sizer->Add(max_lines_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  max_line_sizer->Add(max_lines, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* wrap_around_sizer = new wxBoxSizer(wxHORIZONTAL);
  wrap_around_sizer->Add(wrap_around_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  wrap_around_sizer->Add(wrap_around, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button_);
  system_sizer->AddButton(cancel_button_);
  system_sizer->Realize();

  auto* general_box = new wxStaticBoxSizer(wxVERTICAL,this, L"General");
  general_box->Add(max_line_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  general_box->Add(wrap_around_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);

  auto* time_format_box = new wxStaticBoxSizer(wxVERTICAL,this, L"Time Format");
  time_format_box->Add(show_us, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  time_format_box->Add(show_date, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(general_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(time_format_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  save_button_->SetDefault();
}

} // end namespace
