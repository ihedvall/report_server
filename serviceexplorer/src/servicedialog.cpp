/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <sstream>
#include <filesystem>
#include <boost/program_options.hpp>
#include <wx/config.h>
#include "servicedialog.h"
#include "serviceexplorerid.h"
#include "serviceexplorer.h"

namespace {
  const wxString kStartupList[] = {L"Manual",L"Disabled",L"Automatic"}; //NOLINT
  const wxString kPriorityList[] = {L"Above Normal", L"Below Normal",L"High", L"Idle",L"Normal",L"Real-Time"}; //NOLINT
}

namespace util::service::gui {

wxBEGIN_EVENT_TABLE(ServiceDialog, wxDialog) //NOLINT
    EVT_UPDATE_UI(wxID_APPLY, ServiceDialog::OnUpdateApply)
    EVT_BUTTON(wxID_APPLY, ServiceDialog::OnApply)
    EVT_UPDATE_UI(wxID_SAVE, ServiceDialog::OnUpdateSave)
    EVT_BUTTON(wxID_SAVE, ServiceDialog::OnSave)
    EVT_FILEPICKER_CHANGED(kIdAppPicker, ServiceDialog::OnAppPicker)
    EVT_TEXT_ENTER(kIdName, ServiceDialog::OnNameChange)
    EVT_TEXT(kIdName, ServiceDialog::OnNameChange)
    EVT_FILEPICKER_CHANGED(kIdExePicker, ServiceDialog::OnExePicker)
wxEND_EVENT_TABLE()

ServiceDialog::ServiceDialog(wxWindow *parent, wxWindowID ident, const wxString &title)
: wxDialog(parent,ident,title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE ) {


  save_button_ = new wxButton(this, wxID_SAVE, wxGetStockLabel(wxID_SAVE));
  apply_button_ = new wxButton(this, wxID_APPLY, wxGetStockLabel(wxID_APPLY));
  cancel_button_ = new wxButton(this, wxID_CANCEL, wxGetStockLabel(wxID_CANCEL));

  app_picker_ = new wxFilePickerCtrl(this, kIdAppPicker, wxEmptyString,
                                     L"Service executable (*.exe)|*.exe|All Files (*.*)|*.*", "*.exe",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxFLP_OPEN | wxFLP_FILE_MUST_EXIST);

  exe_picker_ = new wxFilePickerCtrl(this, kIdExePicker, wxEmptyString,
                                     L"Daemon executable (*.exe)|*.exe|All Files (*.*)|*.*", "*.exe",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_USE_TEXTCTRL);
  // Fetch initial directory
  const auto* config = wxConfig::Get();
  if (config != nullptr) {
    const auto app_dir = config->ReadObject("/ServiceDialog/AppPath", wxString());
    if (!app_dir.empty()) {
      app_picker_->SetInitialDirectory(app_dir);
    }
    const auto exe_dir = config->ReadObject("/ServiceDialog/ExePath", wxString());
    if (!exe_dir.empty()) {
      exe_picker_->SetInitialDirectory(exe_dir);
    }
  }
  name_ = new wxTextCtrl(this, kIdName, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                         wxTE_LEFT | wxTE_PROCESS_ENTER,
                         wxTextValidator(wxFILTER_EMPTY | wxFILTER_ASCII, &info_.name));
  name_->SetMaxLength(20);
  name_->SetMinSize({20*8,-1});

  display_name_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                                 0,wxTextValidator(wxFILTER_EMPTY | wxFILTER_ASCII, &info_.display_name));
  display_name_->SetMaxLength(40);
  display_name_->SetMinSize({40*8,-1});

  description_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
      0,wxTextValidator(wxFILTER_NONE, &info_.description));

  app_name_ = new wxTextCtrl(this, wxID_ANY, L"", wxDefaultPosition, wxDefaultSize,
                             wxTE_READONLY | wxTE_LEFT,wxTextValidator(wxFILTER_NONE, &info_.app_name));

  app_path_ = new wxTextCtrl(this, wxID_ANY, L"", wxDefaultPosition, wxDefaultSize,
                             wxTE_READONLY | wxTE_LEFT,wxTextValidator(wxFILTER_NONE, &info_.app_path));

  arguments_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                              0, wxTextValidator(wxFILTER_NONE, &args_));
  arguments_->SetMinSize({60*8,-1});

  dependency_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                         0,wxTextValidator(wxFILTER_ASCII, &info_.dependency));
  dependency_->SetMaxLength(20);
  dependency_->SetMinSize({20*8,-1});

  exe_arguments_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,wxDefaultPosition,wxDefaultSize,
                              0, wxTextValidator(wxFILTER_NONE, &exe_args_));
  exe_arguments_->SetMinSize({60*8,-1});

  startup_ = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,3,kStartupList);


  priority_ = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,6,kPriorityList);


  auto* name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto* display_name_label = new wxStaticText(this, wxID_ANY, L"Display Name:");
  auto* description_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto* app_name_label = new wxStaticText(this, wxID_ANY, L"Application Name:");
  auto* app_path_label = new wxStaticText(this, wxID_ANY, L"Application Path:");
  auto* arg_label = new wxStaticText(this, wxID_ANY, L"Arguments:");
  auto* dependency_label = new wxStaticText(this, wxID_ANY, L"Dependency:");
  auto* exe_name_label = new wxStaticText(this, wxID_ANY, L"Application:");
  auto* exe_argument_label = new wxStaticText(this, wxID_ANY, L"Arguments:");
  auto* startup_label = new wxStaticText(this, wxID_ANY, L"Starts:");
  auto* priority_label = new wxStaticText(this, wxID_ANY, L"Priority:");

  int label_width = 100;
  label_width = std::max(label_width,name_label->GetBestSize().GetX());
  label_width = std::max(label_width, display_name_label->GetBestSize().GetX());
  label_width = std::max(label_width, description_label->GetBestSize().GetX());
  label_width = std::max(label_width, app_name_label->GetBestSize().GetX());
  label_width = std::max(label_width, app_path_label->GetBestSize().GetX());
  label_width = std::max(label_width, arg_label->GetBestSize().GetX());
  label_width = std::max(label_width, dependency_label->GetBestSize().GetX());
  label_width = std::max(label_width, exe_name_label->GetBestSize().GetX());
  label_width = std::max(label_width, exe_argument_label->GetBestSize().GetX());
  label_width = std::max(label_width, startup_label->GetBestSize().GetX());
  label_width = std::max(label_width, priority_label->GetBestSize().GetX());

  auto* name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_label->SetMinSize({label_width, -1});
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* display_name_sizer = new wxBoxSizer(wxHORIZONTAL);
  display_name_label->SetMinSize({label_width, -1});
  display_name_sizer->Add(display_name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  display_name_sizer->Add(display_name_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* description_sizer = new wxBoxSizer(wxHORIZONTAL);
  description_label->SetMinSize({label_width, -1});
  description_sizer->Add(description_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  description_sizer->Add(description_, 1,  wxLEFT | wxRIGHT | wxEXPAND, 5);

  auto* app_name_sizer = new wxBoxSizer(wxHORIZONTAL);
  app_name_label->SetMinSize({label_width, -1});
  app_name_sizer->Add(app_name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  app_name_sizer->Add(app_name_, 1,  wxLEFT | wxEXPAND, 5);
  app_name_sizer->Add(app_picker_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* app_path_sizer = new wxBoxSizer(wxHORIZONTAL);
  app_path_label->SetMinSize({label_width, -1});
  app_path_sizer->Add(app_path_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  app_path_sizer->Add(app_path_, 1, wxLEFT | wxRIGHT | wxEXPAND, 5);

  auto* arg_sizer = new wxBoxSizer(wxHORIZONTAL);
  arg_label->SetMinSize({label_width, -1});
  arg_sizer->Add(arg_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  arg_sizer->Add(arguments_, 1, wxLEFT | wxRIGHT | wxEXPAND, 5);

  auto* dependency_sizer = new wxBoxSizer(wxHORIZONTAL);
  dependency_label->SetMinSize({label_width, -1});
  dependency_sizer->Add(dependency_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  dependency_sizer->Add(dependency_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* exe_name_sizer = new wxBoxSizer(wxHORIZONTAL);
  exe_name_label->SetMinSize({label_width, -1});
  exe_name_sizer->Add(exe_name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  exe_name_sizer->Add(exe_picker_, 1, wxLEFT | wxRIGHT | wxEXPAND, 5);

  auto* exe_arg_sizer = new wxBoxSizer(wxHORIZONTAL);
  exe_argument_label->SetMinSize({label_width, -1});
  exe_arg_sizer->Add(exe_argument_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  exe_arg_sizer->Add(exe_arguments_, 1, wxLEFT | wxEXPAND, 5);

  auto* startup_sizer = new wxBoxSizer(wxHORIZONTAL);
  startup_label->SetMinSize({label_width, -1});
  startup_sizer->Add(startup_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  startup_sizer->Add(startup_, 0,  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* priority_sizer = new wxBoxSizer(wxHORIZONTAL);
  priority_label->SetMinSize({label_width, -1});
  priority_sizer->Add(priority_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT , 5);
  priority_sizer->Add(priority_, 0,  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto* system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button_);
  system_sizer->AddButton(apply_button_);
  system_sizer->AddButton(cancel_button_);
  system_sizer->Realize();

  auto* general_box = new wxStaticBoxSizer(wxVERTICAL,this, L"Service Description");
  general_box->Add(name_sizer, 0, wxALIGN_LEFT | wxTOP | wxBOTTOM | wxEXPAND, 4);
  general_box->Add(display_name_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  general_box->Add(description_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);

  auto* exe_box = new wxStaticBoxSizer(wxVERTICAL,this, L"Service Executable");
  exe_box->Add(app_name_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  exe_box->Add(app_path_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  exe_box->Add(arg_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  exe_box->Add(dependency_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  exe_box->Add(startup_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);

  auto* daemon_box = new wxStaticBoxSizer(wxVERTICAL,this, L"Daemon Executable");
  daemon_box->Add(exe_name_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  daemon_box->Add(exe_arg_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);
  daemon_box->Add(priority_sizer, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 4);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(general_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(exe_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(daemon_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  cancel_button_->SetDefault();

}

void ServiceDialog::OnSave(wxCommandEvent &event) {
  if (!Validate() || !TransferDataFromWindow()) {
    return;
  }

  DoInstall();

  if (IsModal()) {
    EndModal(wxID_SAVE);
  } else {
    SetReturnCode(wxID_SAVE);
    Show(false);
  }
}

void ServiceDialog::OnUpdateSave(wxUpdateUIEvent &event) {
  if (!new_info_) {
    event.Enable(false);
    return;
  }

  const auto name = name_->GetValue();
  if (name.empty()) {
    event.Enable(false);
    return;
  }

  const auto display_name = display_name_->GetValue();
  if (display_name.empty()) {
    event.Enable(false);
    return;
  }
  event.Enable(true);
}

void ServiceDialog::OnApply(wxCommandEvent &event) {
  if (!Validate() || !TransferDataFromWindow()) {
    return;
  }
  DoInstall();
}

void ServiceDialog::OnUpdateApply(wxUpdateUIEvent &event) {
  OnUpdateSave(event);
}

void ServiceDialog::SetInfo(const ServiceInfo &info, bool new_info) {
  info_ = info;
  new_info_ = new_info;
  if (name_ != nullptr) {
    name_->SetEditable(new_info_);
  }
    // Fix argument list
  std::wostringstream temp;
  args_.clear();
  for (const auto& arg : info_.arguments) {
    if (arg.empty()) {
      continue;
    }

    if (!temp.str().empty()) {
      temp << " ";
    }

    if (arg.Find(L' ') != wxNOT_FOUND && arg[0] != L'\"') {
      temp << L'\"' << arg << L'\"';
    } else {
      temp << arg;
    }
  }
  TransferDataToWindow();

  if (name_ != nullptr && new_info_) {
    name_->SetFocus();
  } else if (arguments_ != nullptr) {
    arguments_->SetFocus();
  }

}

const ServiceInfo& ServiceDialog::GetInfo() const {
  return info_;
}

void ServiceDialog::DoInstall() {
  info_.SaveConfig(L"Install");
  if (ServiceHelper::Instance().HaveAllAccess()) {
   InstallService(info_.name.ToStdWstring());
  } else {
    const std::wstring params = L"--install=\"" + info_.name.ToStdWstring() + L"\"";
    wxGetApp().RunAs(GetHWND(), params);
  }
}

void ServiceDialog::OnAppPicker(wxFileDirPickerEvent& event) {
  const auto file = event.GetPath();
  try {
    std::filesystem::path full_name(file.ToStdWstring());
    app_name_->SetValue(full_name.filename().wstring());
    app_path_->SetValue(full_name.parent_path().wstring());
    auto* config = wxConfig::Get();
    if (config != nullptr) {
      config->Write("/ServiceDialog/AppPath", wxString(full_name.parent_path().wstring()));
    }
  } catch (const std::exception&) {
  }


}

/**
 * If this is a service daemon service, the name an the argument should be the same.
 * @param event event generated
 */
void ServiceDialog::OnNameChange(wxCommandEvent&) {
  if (!info_.IsDaemon()) {
    return; // No synchronization needed
  }
  name_->TransferDataFromWindow();
  arguments_->SetValue(info_.name);
}

void ServiceDialog::OnExePicker(wxFileDirPickerEvent& event) {
  const auto file = event.GetPath();
  const auto app_path = app_path_->GetValue();
  try {
    const std::filesystem::path full_name(file.ToStdWstring());
    const std::filesystem::path a_path(app_path.ToStdWstring());
    const auto parent_path = full_name.parent_path();
    if (parent_path == a_path) {
      exe_picker_->SetPath(full_name.filename().wstring());
    } else {
      exe_picker_->SetPath(full_name.wstring());
    }
    auto* config = wxConfig::Get();
    if (config != nullptr) {
      config->Write("/ServiceDialog/ExePath", wxString(full_name.parent_path().wstring()));
    }
  } catch (const std::exception&) {

  }
}
bool ServiceDialog::TransferDataToWindow() {
  std::wostringstream app_arg;
  for (const auto& arg : info_.arguments) {
    const auto spaces = std::ranges::count_if(arg.ToStdWstring(),isspace);
    if (spaces > 0) {
      app_arg << L" \"" << arg << L"\"";
    } else {
      app_arg << arg;
    }
  }
  args_ = app_arg.str();

  std::wostringstream exe_arg;
  for (const auto& exe : info_.exe_arguments) {
    const auto spaces = std::ranges::count_if(exe.ToStdWstring(),isspace);
    if (spaces > 0) {
      exe_arg << L" \"" << exe << L"\"";
    } else {
      exe_arg << exe;
    }
  }
  exe_args_ = exe_arg.str();

  for (int startup = 0; startup < 3; ++startup) {
    auto sel = startup_->GetString(startup);
    if (sel == info_.GetTypeString()) {
      startup_->SetSelection(startup);
      break;
    }
  }

  for (int priority = 0; priority < 6; ++priority) {
    auto sel = priority_->GetString(priority);
    if (sel == info_.GetPriorityString()) {
      priority_->SetSelection(priority);
      break;
    }
  }

  exe_picker_->SetPath(info_.exe_name);

  return wxWindowBase::TransferDataToWindow();
}
bool ServiceDialog::TransferDataFromWindow() {  // Fix the exe_picker and combo boxes
  const auto startup = startup_->GetSelection();
  switch (startup) {
    case 0:
      info_.startup = StartupType::Manual;
      break;

    case 1:
      info_.startup = StartupType::Disabled;
      break;

    default:
      info_.startup = StartupType::Automatic;
      break;
  }

  const auto priority = priority_->GetSelection();
  switch (priority) {
    case 0:
      info_.priority = ABOVE_NORMAL_PRIORITY_CLASS;
      break;

    case 1:
      info_.priority = BELOW_NORMAL_PRIORITY_CLASS;
      break;

    case 2:
      info_.priority = HIGH_PRIORITY_CLASS;
      break;

    case 4:
      info_.priority = NORMAL_PRIORITY_CLASS;
      break;

    case 5:
      info_.priority = REALTIME_PRIORITY_CLASS;
      break;

    case 3:
    default:
      info_.priority = IDLE_PRIORITY_CLASS;
      break;
  }

  info_.exe_name = exe_picker_->GetPath();

  auto ret = wxWindowBase::TransferDataFromWindow();

  info_.exe_arguments.clear();
  try {
    auto argument_list = boost::program_options::split_winmain(exe_args_.ToStdWstring());
    for (const auto& args : argument_list) {
      info_.exe_arguments.emplace_back(args);
    }
  } catch (const std::exception&) {
    ret = false;
  }

  info_.arguments.clear();
  try {
    auto argument_list = boost::program_options::split_winmain(args_.ToStdWstring());
    for (const auto& args : argument_list) {
      info_.arguments.emplace_back(args);
    }
  } catch (const std::exception&) {
    ret = false;
  }
  return ret;
}


} // end namespace