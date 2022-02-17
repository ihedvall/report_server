/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <sstream>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <wx/config.h>
#include <wx/aboutdlg.h>
#include <wx/sizer.h>

#include "mainframe.h"
#include "serviceexplorerid.h"
#include "serviceexplorer.h"
#include "servicedialog.h"
namespace {

// Bitmap indexes
[[maybe_unused]] constexpr int kBluePlay = 0;
[[maybe_unused]] constexpr int kBlueStandby = 1;
[[maybe_unused]] constexpr int kBlueStop = 2;
[[maybe_unused]] constexpr int kGreenPlay = 3;
[[maybe_unused]] constexpr int kGreenStop = 4;
[[maybe_unused]] constexpr int kBlueCircleOn = 5;
[[maybe_unused]] constexpr int kBlueCircleOff = 6;
[[maybe_unused]] constexpr int kGreenCircleOn = 7;
[[maybe_unused]] constexpr int kGrayCircleOff = 8;
}

namespace util::service::gui {

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame) //NOLINT
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_NEW, MainFrame::OnNewService)
    EVT_UPDATE_UI(wxID_OPEN, MainFrame::OnUpdateOpenService)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpenService)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_CHECKBOX(kIdShowDaemon, MainFrame::OnFilterClick)
    EVT_CHECKBOX(kIdHideSvc, MainFrame::OnFilterClick)
    EVT_UPDATE_UI(wxID_DELETE, MainFrame::OnUpdateDeleteService)
    EVT_MENU(wxID_DELETE, MainFrame::OnDeleteService)
    EVT_LIST_ITEM_RIGHT_CLICK(kIdServiceList, MainFrame::OnRightClick)
    EVT_LIST_ITEM_ACTIVATED(kIdServiceList, MainFrame::OnItemActivated)
    EVT_UPDATE_UI(kIdStartService, MainFrame::OnUpdateStartService)
    EVT_MENU(kIdStartService, MainFrame::OnStartService)
    EVT_UPDATE_UI(kIdStopService, MainFrame::OnUpdateStopService)
    EVT_MENU(kIdStopService, MainFrame::OnStopService)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString &title, const wxPoint &start_pos, const wxSize &start_size, bool maximized)
    : wxFrame(nullptr, wxID_ANY, title, start_pos, start_size),
      image_list_(16, 16, false, 9) {
  SetIcon(wxIcon("APP_ICON", wxBITMAP_TYPE_ICO_RESOURCE));
  wxWindow::SetName("ServiceTopWindow");
  wxTopLevelWindowMSW::Maximize(maximized);

  auto *app_config = wxConfig::Get();

  auto *menu_file = new wxMenu;
  menu_file->Append(wxID_EXIT);

  auto *menu_service = new wxMenu;
  menu_service->Append(wxID_NEW);
  menu_service->Append(wxID_OPEN);
  menu_service->AppendSeparator();
  menu_service->Append(wxID_DELETE);

  auto *menu_control = new wxMenu;
  menu_control->Append(kIdStartService, "Start Service");
  menu_control->Append(kIdStopService, "Stop Service");


  // ABOUT
  auto *menu_about = new wxMenu;
  menu_about->Append(kIdOpenLogFile, L"Open Log File");
  menu_about->AppendSeparator();
  menu_about->Append(wxID_ABOUT, wxGetStockLabel(wxID_ABOUT));

  auto *menu_bar = new wxMenuBar;
  menu_bar->Append(menu_file, wxGetStockLabel(wxID_FILE));
  menu_bar->Append(menu_service, L"Configuration");
  menu_bar->Append(menu_control, L"Control");
  menu_bar->Append(menu_about, wxGetStockLabel(wxID_HELP));
  wxFrameBase::SetMenuBar(menu_bar);

  list_view_ = new wxListView(this, kIdServiceList, wxDefaultPosition,
                              wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
  list_view_->AppendColumn(L"Name", wxLIST_FORMAT_LEFT, 150);
  list_view_->AppendColumn(L"Display Name", wxLIST_FORMAT_LEFT, 250);
  list_view_->AppendColumn(L"Description", wxLIST_FORMAT_LEFT, 400);
  list_view_->AppendColumn(L"Service App", wxLIST_FORMAT_LEFT, 150);
  list_view_->AppendColumn(L"Type", wxLIST_FORMAT_LEFT, 100);
  list_view_->AppendColumn(L"Daemon", wxLIST_FORMAT_LEFT, 100);
  list_view_->AppendColumn(L"Priority", wxLIST_FORMAT_LEFT, 100);


  show_serviced_ = new wxCheckBox(this, kIdShowDaemon, L"Show Only Service Daemons");
  hide_svc_ = new wxCheckBox(this, kIdHideSvc, L"Hide Window Services");

  show_serviced_->SetValue(app_config->ReadBool("/MainWin/ShowDaemon", false));
  hide_svc_->SetValue(app_config->ReadBool("/MainWin/HideSvc", true));

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(list_view_, 1, wxEXPAND | wxALL, 0);

  auto *filter_sizer = new wxBoxSizer(wxHORIZONTAL);
  filter_sizer->Add(show_serviced_, 0, wxLEFT, 5);
  filter_sizer->Add(hide_svc_, 0, wxLEFT, 5);

  main_sizer->Add(filter_sizer, 0, wxALIGN_LEFT | wxLEFT | wxBOTTOM, 5);

  SetSizer(main_sizer);

  image_list_.Add(wxBitmap("LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  list_view_->SetImageList(&image_list_, wxIMAGE_LIST_SMALL);
}

void MainFrame::OnExit(wxCommandEvent & WXUNUSED(event)) {
  Close(true);
}

void MainFrame::OnClose(wxCloseEvent &event) {

  // If the window is minimized. Do not save as last position
  auto *app_config = wxConfig::Get();
  if (!IsIconized()) {
    bool maximized = IsMaximized();
    wxPoint end_pos = GetPosition();
    wxSize end_size = GetSize();
    if (maximized) {
      app_config->Write("/MainWin/Max", maximized);
    } else {
      app_config->Write("/MainWin/X", end_pos.x);
      app_config->Write("/MainWin/Y", end_pos.y);
      app_config->Write("/MainWin/XWidth", end_size.x);
      app_config->Write("/MainWin/YWidth", end_size.y);
      app_config->Write("/MainWin/Max", maximized);
    }

  }
  if (show_serviced_ && hide_svc_) {
    app_config->Write("/MainWin/ShowDaemon", show_serviced_->GetValue());
    app_config->Write("/MainWin/HideSvc", hide_svc_->GetValue());
  }
  event.Skip(true);
}

void MainFrame::OnAbout(wxCommandEvent &) { //NOLINT
  wxAboutDialogInfo info;
  info.SetName("Service Explorer");
  info.SetVersion("1.0");
  info.SetDescription("Service Explorer");

  wxArrayString devs;
  devs.push_back("Ingemar Hedvall");
  info.SetDevelopers(devs);

  info.SetCopyright("(C) 2022 Ingemar Hedvall");
  info.SetLicense("MIT License (https://opensource.org/licenses/MIT)\n"
                  "Copyright 2022 Ingemar Hedvall\n"
                  "\n"
                  "Permission is hereby granted, free of charge, to any person obtaining a copy of this\n"
                  "software and associated documentation files (the \"Software\"),\n"
                  "to deal in the Software without restriction, including without limitation the rights to use, copy,\n"
                  "modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,\n"
                  "and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n"
                  "\n"
                  "The above copyright notice and this permission notice shall be included in all copies or substantial\n"
                  "portions of the Software.\n"
                  "\n"
                  "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\n"
                  "INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR\n"
                  "PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,\n"
                  "DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR\n"
                  "IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE."
  );
  wxAboutBox(info);
}

void MainFrame::RedrawServiceList() {
  wxBusyCursor wait;
  if (!list_view_) {
    return;
  }
  list_view_->DeleteAllItems();
  const auto &service_list = ServiceHelper::Instance().GetServiceList();

  long line = 0;
  for (const auto &itr: service_list) {
    const auto &info = itr.second;
    if (hide_svc_ && hide_svc_->GetValue()) {
      // Skip empty and svchost.exe application names
      if (info.app_name.empty()) {
        continue;
      }
      if (boost::iequals(info.app_name.ToStdWstring(), L"svchost.exe")) {
        continue;
      }
      if (boost::iequals(info.app_name.ToStdWstring(), L"lsass.exe")) {
        continue;
      }
      if (boost::iequals(info.app_name.ToStdWstring(), L"dllhost.exe")) {
        continue;
      }
    }
    if (show_serviced_ && show_serviced_->GetValue()) {
      // show only service daemons
      if (!boost::iequals(info.app_name.ToStdWstring(), L"serviced.exe")) {
        continue;
      }
    }
    auto bmp = info.IsRunning() ? kGreenCircleOn : kGrayCircleOff;
    auto index = list_view_->InsertItem(line++, info.name, bmp);
    list_view_->SetItem(index, 1, info.display_name);
    list_view_->SetItem(index, 2, info.description);
    list_view_->SetItem(index, 3, info.app_name);
    list_view_->SetItem(index, 4, info.GetTypeString());
    if (info.IsDaemon() && !info.exe_name.empty()) {
      std::wstring exe_name;
      try {
        std::filesystem::path fullname(info.exe_name.ToStdWstring());
        auto filename = fullname.filename();
        exe_name = filename.c_str();
      } catch (const std::exception &) {
        exe_name = L"";
      }
      list_view_->SetItem(index, 5, exe_name);

      list_view_->SetItem(index, 6, info.GetPriorityString());

    }

  }
}

void MainFrame::OnFilterClick(wxCommandEvent &event) {
  RedrawServiceList();
}

void MainFrame::OnNewService(wxCommandEvent &event) {
  if (list_view_ == nullptr) {
    return;
  }
  ServiceInfo info;
  auto serviced = wxGetApp().GetServiceDaemon();

  try {
    std::filesystem::path full_path(serviced);
    info.app_path = full_path.parent_path().wstring();
    info.app_name = full_path.filename().wstring();
  } catch (const std::exception &) {
  }

  const auto item = list_view_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item >= 0 && info.app_name.empty()) {
    const auto name = list_view_->GetItemText(item);
    const auto &service_list = ServiceHelper::Instance().GetServiceList();
    const auto itr = service_list.find(name.ToStdWstring());
    if (itr != service_list.cend()) {
      info = itr->second;
      info.name.clear();
      info.display_name.clear();
    }
  }

  ServiceDialog dlg(this, wxID_ANY, L"Service - New");
  dlg.SetInfo(info, true);
  dlg.ShowModal();
  ServiceHelper::Instance().FetchServices();
  RedrawServiceList();
  SelectItem(dlg.GetInfo().name);
}

void MainFrame::OnUpdateOpenService(wxUpdateUIEvent &event) {
  if (list_view_ != nullptr && list_view_->GetSelectedItemCount() == 1) {
    event.Enable(true);
  } else {
    event.Enable(false);
  }
}

void MainFrame::OnOpenService(wxCommandEvent &event) {
  if (list_view_ == nullptr) {
    return;
  }
  const auto item = list_view_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item < 0) {
    return;
  }
  const auto name = list_view_->GetItemText(item);
  const auto &service_list = ServiceHelper::Instance().GetServiceList();
  const auto itr = service_list.find(name.ToStdWstring());
  if (itr == service_list.cend()) {
    return;
  }
  std::wostringstream title;
  title << L"Service - " << name;

  ServiceDialog dlg(this, wxID_ANY, title.str());
  dlg.SetInfo(itr->second, false);
  dlg.ShowModal();
}

void MainFrame::OnUpdateDeleteService(wxUpdateUIEvent &event) {
  if (list_view_ == nullptr) {
    event.Enable(false);
    return;
  }

  if (list_view_->GetSelectedItemCount() != 1) {
    event.Enable(false);
    return;
  }
  event.Enable(true);
}

void MainFrame::OnDeleteService(wxCommandEvent &event) {
  if (list_view_ == nullptr) {
    return;
  }
  const auto item = list_view_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item < 0) {
    return;
  }
  const auto name = list_view_->GetItemText(item);
  const auto &service_list = ServiceHelper::Instance().GetServiceList();
  const auto itr = service_list.find(name.ToStdWstring());
  if (itr == service_list.cend()) {
    return;
  }
  if (itr->second.IsRunning()) {
    std::wostringstream temp;
    temp << L"The service is running. Do you really want to delete this service?" << std::endl
         << L"Service: " << itr->first << std::endl;

    wxMessageDialog ask(this, temp.str(), L"Uninstall Service", wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_WARNING);
    const auto ret = ask.ShowModal();
    if (ret != wxID_YES) {
      return;
    }
  } else if (!itr->second.IsDaemon()) {
    std::wostringstream temp;
    temp << L"The service is an Windows service. Do you really want to delete this service?" << std::endl
         << L"Service: " << itr->first << std::endl;

    wxMessageDialog ask(this, temp.str(), L"Uninstall Service", wxYES_NO | wxNO_DEFAULT | wxCENTRE| wxICON_WARNING);
    const auto ret = ask.ShowModal();
    if (ret != wxID_YES) {
      return;
    }
  } else {
    std::wostringstream temp;
    temp << L"Do you really want to delete this service?" << std::endl
         << L"Service: " << itr->first << std::endl;

    wxMessageDialog ask(this, temp.str(), L"Uninstall Service", wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_WARNING);
    const auto ret = ask.ShowModal();
    if (ret != wxID_YES) {
      return;
    }
  }

  if (ServiceHelper::Instance().HaveAllAccess()) {
    UninstallService( name.ToStdWstring());
  } else {
    const std::wstring params = L"--uninstall=\"" + name.ToStdWstring() + L"\"";
    wxGetApp().RunAs(GetHWND(), params);
  }
  ServiceHelper::Instance().FetchServices();
  RedrawServiceList();
}

void MainFrame::OnRightClick(wxListEvent &event) {
  wxMenu menu;
  menu.Append(kIdStartService, "Start Service");
  menu.Append(kIdStopService, "Stop Service");
  menu.Append(wxID_OPEN);
  menu.AppendSeparator();
  menu.Append(wxID_NEW);
  menu.Append(wxID_DELETE);
  menu.AppendSeparator();
  menu.Append(kIdOpenLogFile, "Open Log File");

  PopupMenu(&menu);
}

void MainFrame::OnItemActivated(wxListEvent &event) {
  if (list_view_ == nullptr) {
    return;
  }
  const auto &item = event.GetItem();
  const auto name = list_view_->GetItemText(item);
  const auto &service_list = ServiceHelper::Instance().GetServiceList();
  const auto itr = service_list.find(name.ToStdWstring());
  if (itr == service_list.cend()) {
    return;
  }
  std::wostringstream title;
  title << L"Service - " << name;

  ServiceDialog dlg(this, wxID_ANY, title.str());
  dlg.SetInfo(itr->second, false);
  dlg.ShowModal();
}

void MainFrame::OnUpdateStartService(wxUpdateUIEvent &event) {
  if (list_view_ == nullptr) {
    event.Enable(false);
    return;
  }

  if (list_view_->GetSelectedItemCount() != 1) {
    event.Enable(false);
    return;
  }

  const auto &service_list = ServiceHelper::Instance().GetServiceList();
  const auto item = list_view_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item < 0) {
    event.Enable(false);
    return;
  }

  const auto name = list_view_->GetItemText(item);
  const auto itr = service_list.find(name.ToStdWstring());
  if (itr == service_list.cend()) {
    event.Enable(false);
    return;
  }

  if (itr->second.IsRunning()) {
    event.Enable(false);
    return;
  }
  event.Enable(true);
}

void MainFrame::OnStartService(wxCommandEvent &event) {
  if (list_view_ == nullptr) {
    return;
  }
  const auto item = list_view_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item < 0) {
    return;
  }
  const auto name = list_view_->GetItemText(item);
  const auto &service_list = ServiceHelper::Instance().GetServiceList();
  const auto itr = service_list.find(name.ToStdWstring());
  if (itr == service_list.cend()) {
    return;
  }
  if (ServiceHelper::Instance().HaveControlAccess()) {
    StartServiceInt(name.ToStdWstring());
  } else {
    const std::wstring params = L"--start=\"" + name.ToStdWstring() + L"\"";
    wxGetApp().RunAs(GetHWND(), params);
  }
  ServiceHelper::Instance().FetchServices();
  RedrawServiceList();
  SelectItem(name);
}

void MainFrame::OnUpdateStopService(wxUpdateUIEvent &event) {
  if (list_view_ == nullptr) {
    event.Enable(false);
    return;
  }

  if (list_view_->GetSelectedItemCount() != 1) {
    event.Enable(false);
    return;
  }

  const auto &service_list = ServiceHelper::Instance().GetServiceList();
  const auto item = list_view_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item < 0) {
    event.Enable(false);
    return;
  }

  const auto name = list_view_->GetItemText(item);
  const auto itr = service_list.find(name.ToStdWstring());
  if (itr == service_list.cend()) {
    event.Enable(false);
    return;
  }

  if (!itr->second.IsRunning()) {
    event.Enable(false);
    return;
  }
  event.Enable(true);
}

void MainFrame::OnStopService(wxCommandEvent &event) {
  if (list_view_ == nullptr) {
    return;
  }
  const auto item = list_view_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (item < 0) {
    return;
  }
  const auto name = list_view_->GetItemText(item);
  const auto &service_list = ServiceHelper::Instance().GetServiceList();
  const auto itr = service_list.find(name.ToStdWstring());
  if (itr == service_list.cend()) {
    return;
  }
  if (ServiceHelper::Instance().HaveControlAccess()) {
    StopServiceInt(name.ToStdWstring());
  } else {
    const std::wstring params = L"--stop=\"" + name.ToStdWstring() + L"\"";
    wxGetApp().RunAs(GetHWND(), params);
  }
  ServiceHelper::Instance().FetchServices();
  RedrawServiceList();
  SelectItem(name);
}

void MainFrame::SelectItem(const wxString &name) {
  if (list_view_ == nullptr) {
    return;
  }
  const auto item = list_view_->FindItem(-1,name);
  if (item < 0) {
    return;
  }
  list_view_->Select(item);
  list_view_->EnsureVisible(item);
}

} // end namespace