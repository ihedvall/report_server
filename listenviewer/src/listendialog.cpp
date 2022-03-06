/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <wx/config.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>
#include <wx/valgen.h>
#include "listendialog.h"
#include "listenviewerid.h"
namespace {
constexpr int kActiveBmp = 5;
constexpr int kNotActiveBmp = 6;
}

namespace util::log::gui {

wxBEGIN_EVENT_TABLE(ListenDialog, wxDialog) //NOLINT
    EVT_LIST_ITEM_ACTIVATED(kIdSelectionList, ListenDialog::OnActivateChange)
    EVT_LIST_ITEM_RIGHT_CLICK(kIdSelectionList, ListenDialog::OnRightClick)
    EVT_MENU(kIdEnable, ListenDialog::OnEnable)
    EVT_MENU(kIdDisable, ListenDialog::OnDisable)
wxEND_EVENT_TABLE()

ListenDialog::ListenDialog(wxWindow *parent)
    : wxDialog(parent,wxID_ANY,L"Select Listen Servers", wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX ),
      image_list_(16, 16, false, 9) {

  list_ = new wxListView(this, kIdSelectionList, wxDefaultPosition, {800, 400},wxLC_REPORT | wxLC_SINGLE_SEL );
  list_->AppendColumn(L"Name", wxLIST_FORMAT_LEFT, 150);
  list_->AppendColumn(L"Port", wxLIST_FORMAT_LEFT, 50);
  list_->AppendColumn(L"Host", wxLIST_FORMAT_LEFT, 100);
  list_->AppendColumn(L"Description", wxLIST_FORMAT_LEFT, 500);


  image_list_.Add(wxBitmap("LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  list_->SetImageList(&image_list_, wxIMAGE_LIST_SMALL);


  auto* remote_name = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                                  wxTextValidator(wxFILTER_NONE, &remote_name_));
  remote_name->SetMinSize({20*10,-1});

  auto* remote_description = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                                     wxTextValidator(wxFILTER_NONE, &remote_description_));
  remote_description->SetMinSize({20*10,-1});

  auto* remote_host = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                                     wxTextValidator(wxFILTER_NONE, &remote_host_));;
  remote_host->SetMinSize({20*10,-1});

  auto remote_port = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize, 0,
                                    wxIntegerValidator(&remote_port_));
  remote_port->SetMaxLength(5);
  remote_port->SetMinSize({5*10,-1});

  auto* ok_button_ = new wxButton(this, wxID_OK, wxGetStockLabel(wxID_OK));
  auto* cancel_button_ = new wxButton(this, wxID_CANCEL, wxGetStockLabel(wxID_CANCEL));

  auto* remote_name_label = new wxStaticText(this, wxID_ANY, L"Display Name:");
  auto* remote_desc_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto* remote_host_label = new wxStaticText(this, wxID_ANY, L"Host:");
  auto* remote_port_label = new wxStaticText(this, wxID_ANY, L"IP Port:");

  int label_width = 100;
  label_width = std::max(label_width,remote_name_label->GetBestSize().GetX());
  label_width = std::max(label_width,remote_desc_label->GetBestSize().GetX());
  label_width = std::max(label_width, remote_host_label->GetBestSize().GetX());
  label_width = std::max(label_width, remote_port_label->GetBestSize().GetX());

  remote_name_label->SetMinSize({label_width, -1});
  remote_desc_label->SetMinSize({label_width, -1});
  remote_host_label->SetMinSize({label_width, -1});
  remote_port_label->SetMinSize({label_width, -1});

  auto* remote_name_sizer = new wxBoxSizer(wxHORIZONTAL);
  remote_name_sizer->Add(remote_name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  remote_name_sizer->Add(remote_name, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* remote_desc_sizer = new wxBoxSizer(wxHORIZONTAL);
  remote_desc_sizer->Add(remote_desc_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  remote_desc_sizer->Add(remote_description, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* remote_host_sizer = new wxBoxSizer(wxHORIZONTAL);
  remote_host_sizer->Add(remote_host_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  remote_host_sizer->Add(remote_host, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* remote_port_sizer = new wxBoxSizer(wxHORIZONTAL);
  remote_port_sizer->Add(remote_port_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  remote_port_sizer->Add(remote_port, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(ok_button_);
  system_sizer->AddButton(cancel_button_);
  system_sizer->Realize();

  auto* local_server_box = new wxStaticBoxSizer(wxVERTICAL,this, L"Local Listen Servers");
  local_server_box->Add(list_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 1);

  auto* remote_server_box = new wxStaticBoxSizer(wxVERTICAL,this, L"Remote Listen Server");
  remote_server_box->Add(remote_name_sizer, 0, wxALIGN_LEFT | wxALL | wxEXPAND , 2);
  remote_server_box->Add(remote_desc_sizer, 0, wxALIGN_LEFT | wxALL | wxEXPAND , 2);
  remote_server_box->Add(remote_host_sizer, 0, wxALIGN_LEFT | wxALL | wxEXPAND , 2);
  remote_server_box->Add(remote_port_sizer, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 2);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(local_server_box, 1,  wxALIGN_LEFT| wxALL | wxEXPAND, 4);
  main_sizer->Add(remote_server_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  ok_button_->SetDefault();
}

bool ListenDialog::TransferDataToWindow() {
  if (list_ != nullptr) {
    const auto selected = list_->GetFirstSelected();
    list_->DeleteAllItems();
    for (long item = 0; item < static_cast<long>(selection_list_.size()); ++item) {
      const auto& listen = selection_list_[item];
      list_->InsertItem(item,listen.name,listen.active ? kActiveBmp : kNotActiveBmp);
      list_->SetItem(item,1,std::to_wstring(listen.port));
      list_->SetItem(item,2,listen.host);
      list_->SetItem(item,3,listen.description);
    }
    if (selected >= 0) {
      list_->SetItemState(selected, wxLIST_STATE_SELECTED , wxLIST_STATE_SELECTED);
      list_->EnsureVisible(selected);
    }
  }
  return wxWindowBase::TransferDataToWindow();
}

bool ListenDialog::TransferDataFromWindow() {
  const auto ret = wxWindowBase::TransferDataFromWindow();
  if (remote_port_ > 0 && !remote_name_.empty()) {
    // Add it to the list as active
    ListenSelect remote = {true,
                           remote_name_.ToStdWstring(),
                           remote_host_.ToStdWstring(),
                           remote_description_.ToStdWstring(),
                           remote_port_};
    selection_list_.emplace_back(remote);
  }
  return ret;
}

void ListenDialog::OnActivateChange(wxListEvent &event) {
  const auto index = event.GetIndex();
  if (index < 0 || index >= selection_list_.size() || list_ == nullptr) {
    return;
  }
  auto& listen = selection_list_[index];
  if (listen.active) {
    listen.active = false;
  } else {
    listen.active = true;
  }
  list_->SetItemImage(index,listen.active ? kActiveBmp : kNotActiveBmp);
}

void ListenDialog::OnRightClick(wxListEvent &event) {
  wxMenu menu;
  menu.Append(kIdEnable, L"Enable");
  menu.Append(kIdDisable, L"Disable");
  PopupMenu(&menu);
}

void ListenDialog::OnEnable(wxCommandEvent &event) {
  if (list_ == nullptr || list_->GetSelectedItemCount() != 1) {
    return;
  }
  const auto index = list_->GetFirstSelected();
  if (index < 0 || index >= selection_list_.size()) {
    return;
  }
  auto& listen = selection_list_[index];
  listen.active = true;
  list_->SetItemImage(index,listen.active ? kActiveBmp : kNotActiveBmp);
}

void ListenDialog::OnDisable(wxCommandEvent &event) {
  if (list_ == nullptr || list_->GetSelectedItemCount() != 1) {
    return;
  }
  const auto index = list_->GetFirstSelected();
  if (index < 0 || index >= selection_list_.size()) {
    return;
  }
  auto& listen = selection_list_[index];
  listen.active = false;
  list_->SetItemImage(index,listen.active ? kActiveBmp : kNotActiveBmp);
}


} // end namespace
