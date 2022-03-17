/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <wx/config.h>
#include <wx/aboutdlg.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/srchctrl.h>
#include <wx/choice.h>
#include "util/stringutil.h"
#include "util/listenconfig.h."
#include "../../utillib/src/listenclient.h"
#include "mainframe.h"
#include "listenviewerid.h"
#include "listenviewer.h"
#include "settingdialog.h"
#include "listendialog.h"
#


using namespace util::log::detail;
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

namespace util::log::gui {

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame) //NOLINT
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_SETUP, MainFrame::OnSetting)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_RADIOBOX(kIdAutoScroll, MainFrame::OnAutoScroll)
    EVT_BUTTON(kIdClearList, MainFrame::OnClearList)
    EVT_BUTTON(kIdSettings, MainFrame::OnSetting)
    EVT_TIMER(kIdMainTimer, MainFrame::OnMainTimer)
    EVT_SIZE(MainFrame::OnSizeChange)
    EVT_SEARCHCTRL_SEARCH_BTN(kIdWildCard, MainFrame::OnWildcardChange)
    EVT_SEARCHCTRL_CANCEL_BTN(kIdWildCard, MainFrame::OnWildcardReset)
    EVT_TEXT_ENTER(kIdWildCard, MainFrame::OnWildcardChange)
    EVT_CHOICE(kIdLogLevel, MainFrame::OnLogLevel)
    EVT_LIST_ITEM_ACTIVATED(kIdListenList, MainFrame::OnClientList)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString &title, const wxPoint &start_pos, const wxSize &start_size, bool maximized)
    : wxFrame(nullptr, wxID_ANY, title, start_pos, start_size),
      image_list_(16, 16, false, 9),
      message_buffer_(10),
      main_timer_(this, kIdMainTimer) {
  SetIcon(wxIcon("APP_ICON", wxBITMAP_TYPE_ICO_RESOURCE));
  wxWindow::SetName("ListenTopWindow");
  wxTopLevelWindowMSW::Maximize(maximized);

  auto *app_config = wxConfig::Get();
  app_config->Read("/Setting/MaxLines", &max_lines_);
  app_config->Read("/Setting/WrapAround", &wrap_around_);
  bool show_us = false;
  app_config->Read("/Setting/ShowUs", &show_us);
  bool show_date = false;
  app_config->Read("/Setting/ShowDate", &show_date);

  message_buffer_.set_capacity(max_lines_);

  auto *menu_file = new wxMenu;
  menu_file->Append(wxID_EXIT);

  auto *menu_listen = new wxMenu;
  menu_listen->Append(wxID_OPEN);
  menu_listen->AppendSeparator();
  menu_listen->Append(wxID_DELETE);

  // ABOUT
  auto *menu_about = new wxMenu;
  menu_about->Append(wxID_SETUP, L"Settings");
  menu_about->Append(kIdOpenLogFile, L"Open Log File");

  menu_about->AppendSeparator();
  menu_about->Append(wxID_ABOUT);

  auto *menu_bar = new wxMenuBar;
  menu_bar->Append(menu_file, wxGetStockLabel(wxID_FILE));
  menu_bar->Append(menu_listen, L"Listen Servers");
  menu_bar->Append(menu_about, wxGetStockLabel(wxID_HELP));
  wxFrameBase::SetMenuBar(menu_bar);

  message_list_ = new ListenListView(this, message_buffer_);
  message_list_->ShowDate(show_date);
  message_list_->ShowMicroSeconds(show_us);
  message_list_->AppendColumn(L"Time", wxLIST_FORMAT_LEFT, (show_date ? 150 : 80));
  message_list_->AppendColumn(L"Label", wxLIST_FORMAT_LEFT, 80);
  message_list_->AppendColumn(L"Text", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(message_list_, 1, wxEXPAND | wxALL, 0);

  wxSize list_size(300,40);

  active_list_ = new wxListView(this, kIdListenList, wxDefaultPosition,
                                list_size, wxLC_SMALL_ICON | wxLC_SINGLE_SEL);

  const wxString scroll_list[2] = {L"Auto", L"Manual"};
  auto* auto_scroll_box = new wxRadioBox(this, kIdAutoScroll, L"Auto Scrolling",wxDefaultPosition,wxDefaultSize,
                                 2, scroll_list);
  auto_scroll_box->Enable(0U);

  auto* clear_list = new wxButton(this, kIdClearList, L"Clear List");
  auto* settings = new wxButton(this, kIdSettings, L"Settings");
  auto* control_sizer = new wxBoxSizer(wxVERTICAL);
  control_sizer->Add(clear_list, wxALL, 0);
  control_sizer->Add(settings, wxALL, 0);

  log_level_ = new wxChoice(this, kIdLogLevel);
  log_level_->SetMinSize({200,-1});
  log_level_->Append(L"Default Log Level");
  log_level_->SetSelection(0);

  auto* log_level_box = new wxStaticBoxSizer(wxVERTICAL, this, L"Log Level");
  log_level_box->Add(log_level_,0, wxALL, 0);

  auto* wildcard = new wxSearchCtrl(this, kIdWildCard, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_LEFT);
  wildcard->SetDescriptiveText(L"Filter");
  wildcard->ShowCancelButton(true);
  wildcard->ShowSearchButton(true);

  auto* wildcard_sizer = new wxStaticBoxSizer(wxVERTICAL, this,  L"Wildcard Filter");
  wildcard_sizer->Add(wildcard,0, wxALL, 0);

  auto *filter_sizer = new wxBoxSizer(wxHORIZONTAL);
  filter_sizer->Add(auto_scroll_box, 0, wxLEFT,5);
  filter_sizer->Add(log_level_box, 0, wxLEFT | wxRIGHT, 1);
  filter_sizer->Add(wildcard_sizer, 0, wxLEFT | wxRIGHT, 1);
  filter_sizer->Add(active_list_, 1, wxLEFT | wxRIGHT | wxEXPAND, 1);
  filter_sizer->Add(control_sizer, 0, wxRIGHT,5);

  main_sizer->Add(filter_sizer, 0, wxALIGN_LEFT | wxALL | wxEXPAND , 0);

  SetSizerAndFit(main_sizer);

  image_list_.Add(wxBitmap("LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  active_list_->SetImageList(&image_list_, wxIMAGE_LIST_SMALL);

  if (client_list_.empty()) {
    wxCommandEvent dummy;
    OnOpen(dummy);
  }
  RedrawMessageList();
  main_timer_.Start(50);
}

void MainFrame::OnExit(wxCommandEvent & WXUNUSED(event)) {
  Close(true);
}


void MainFrame::OnAbout(wxCommandEvent &) { //NOLINT
  wxAboutDialogInfo info;
  info.SetName("Listen Viewer");
  info.SetVersion("1.0");
  info.SetDescription("Listen Window Application");

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

void MainFrame::OnSetting(wxCommandEvent& ) {
  if (message_list_ == nullptr) {
    return;
  }
  SettingDialog dialog(this);
  dialog.max_lines_ = max_lines_;
  dialog.wrap_around_ = wrap_around_;
  dialog.show_us_ = message_list_->ShowMicroSeconds();
  dialog.show_date_ = message_list_->ShowDate();
  auto ret = dialog.ShowModal();
  if (ret == wxID_OK) {
    max_lines_ = dialog.max_lines_;
    wrap_around_ = dialog.wrap_around_;
    message_list_->ShowMicroSeconds(dialog.show_us_);
    message_list_->ShowDate(dialog.show_date_);
    auto *app_config = wxConfig::Get();
    app_config->Write("/Setting/MaxLines", max_lines_);
    app_config->Write("/Setting/WrapAround", wrap_around_);
    app_config->Write("/Setting/ShowUs", message_list_->ShowMicroSeconds());
    app_config->Write("/Setting/ShowDate", message_list_->ShowDate());
  }
}

void MainFrame::OnOpen(wxCommandEvent& ) {
  ListenDialog dialog(this);

  // Add active first
  for (const auto& active : client_list_) {
    if (active) {
      ListenSelect select;
      select.active = true;
      select.name = wxString(active->Name()).ToStdWstring();
      select.host = wxString(active->HostName()).ToStdWstring();
      select.description = wxString(active->Description()).ToStdWstring();
      select.port = active->Port();
      dialog.selection_list_.emplace_back(select);
    }
  }

  const auto config_list = util::log::GetListenConfigList();
  for (const auto& avail : config_list) {
      const auto exist = std::ranges::find_if(client_list_, [&] (const auto& listen) {
        return listen && listen->Port() == avail.port &&
            (listen->HostName().empty() || listen->HostName() == "127.0.0.1");
      });
      if (exist != client_list_.cend()) {
        continue;
      }
      ListenSelect select;
      select.active = false;
      select.name = wxString(avail.name).ToStdWstring();
      select.host = L"127.0.0.1";
      select.description = wxString(avail.description).ToStdWstring();
      select.port = avail.port;
      dialog.selection_list_.emplace_back(select);

  }

  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  // Check if any new added
  for (const auto& add : dialog.selection_list_) {
    if (!add.active) {
      continue;
    }
    const auto exist = std::ranges::find_if(client_list_, [&] (const auto& itr) {
      wxString host(add.host);
      return itr && util::string::IEquals(itr->HostName(),host.ToStdString()) && itr->Port() == add.port;
    });
    if (exist == client_list_.cend() && add.port > 0) {
      wxString host(add.host);
      auto client = std::make_unique<util::log::detail::ListenClient>(host.ToStdString(), add.port);
      client->Name(wxString(add.name).ToStdString());
      client->Description(wxString(add.description).ToStdString());
      client_list_.push_back(std::move(client));
    }
  }

  // Check if any clients need to be removed added
  for (auto del = client_list_.begin(); del != client_list_.end(); /* No ++del here */) {
    auto* listen = del->get();
    const auto exist = std::ranges::find_if(dialog.selection_list_, [&] (const auto& sel) {
      wxString host(sel.host);
       const auto match = listen != nullptr && sel.active &&
          util::string::IEquals(listen->HostName(),host.ToStdString()) &&
          listen->Port() == sel.port;
      return match;
    });
    if (exist == dialog.selection_list_.cend()) {
      del = client_list_.erase(del);
    } else {
      ++del;
    }
  }
  RedrawActiveList();
}

void MainFrame::RedrawMessageList() {
  wxBusyCursor wait;
  if (message_list_ == nullptr) {
    return;
  }
  message_list_->SetItemCount(static_cast<long>(message_buffer_.size()));
  if (auto_scroll_ && !message_buffer_.empty()) {
    message_list_->EnsureVisible(static_cast<long>(message_buffer_.size() - 1));
  }
}

void MainFrame::RedrawActiveList() {
  wxBusyCursor wait;
  if (active_list_ == nullptr) {
    return;
  }
  active_list_->DeleteAllItems();
  long item = 0;
  for (const auto& listen : client_list_) {
    if (!listen) {
      continue;
    }
    if (listen->Active()) {
      active_list_->InsertItem(item, listen->Name(), listen->Active() ? kGreenPlay : kGreenStop);
    }
  }
}

void MainFrame::OnAutoScroll(wxCommandEvent& event) {
  auto_scroll_ = event.GetInt() == 0;
}

void MainFrame::OnClearList(wxCommandEvent &event) {
  message_buffer_.clear();
  RedrawMessageList();
}

void MainFrame::OnMainTimer(wxTimerEvent& ) {
  for (auto& client : client_list_) {
    if (!client) {
      continue;
    }
    HandleMessages(*client);
    HandleStatusBitmap(*client);
  }
}

void MainFrame::HandleMessages(ListenClient& client) {
  std::unique_ptr<ListenMessage> message;
  for (bool more = client.GetMessage(message); more ; more = client.GetMessage(message)) {
    if (!message || !client.Active()) {
      continue;
    }
    switch(message->type_) {
      case ListenMessageType::LogLevelText: {
        const auto* level_list = dynamic_cast<const LogLevelTextMessage*>(message.get());
        if (level_list != nullptr && log_level_ != nullptr &&
            !level_list->log_level_text_list_.empty() && log_level_->GetCount() <= 1) {
          log_level_list_ = level_list->log_level_text_list_;
          log_level_->Clear();
          for(const auto& itr : log_level_list_) {
            log_level_->Append(itr.second);
          }
          log_level_->SetSelection(0);
        }
        break;
      }

      case ListenMessageType::TextMessage: {
        const auto* text_msg = dynamic_cast<const ListenTextMessage*>(message.get());
        if (text_msg != nullptr && FilterMessage(*text_msg)) {
          break;
        }
        message_buffer_.push_back(std::move(message));
        RedrawMessageList();
        break;
      }

      case ListenMessageType::LogLevel: {
        const auto* level = dynamic_cast<const LogLevelMessage*>(message.get());
        if (level != nullptr && log_level_ != nullptr) {
          int selection = 0;
          for (const auto& itr : log_level_list_) {
            if (level->log_level_ == itr.first) {
              if (log_level_->GetSelection() != selection) {
                log_level_->SetSelection(selection);
              }
              break;
            }
            ++selection;
          }
        }
        break;
      }

      default:
        break;
    }
    message.reset();
  }
}

void MainFrame::OnSizeChange(wxSizeEvent& event) {
  if (message_list_ != nullptr) {
    message_list_->SetColumnWidth(2,wxLIST_AUTOSIZE_USEHEADER);
  }
  event.Skip(true);
}

void MainFrame::OnWildcardChange(wxCommandEvent &event) {
  wildcard_ = event.GetString().ToStdString();
}

void MainFrame::OnWildcardReset(wxCommandEvent &event) {
  wildcard_ = {};
}

void MainFrame::HandleStatusBitmap(const ListenClient &client) {
  int bitmap = kGreenPlay;
  if (!client.IsConnected()) {
    bitmap = kGrayCircleOff;
  } else if (!client.Active()) {
    bitmap = kGreenStop;
  }
  const wxString name(client.Name());
  if (active_list_ != nullptr) {
    const auto item = active_list_->FindItem(-1,name);
    if (item >= 0) {
      wxListItem info;
      info.SetId(item);
      active_list_->GetItem(info);
      if (info.GetImage() != bitmap) {
        info.SetImage(bitmap);
        active_list_->SetItem(info);
      }
    }
  }
}

void MainFrame::OnLogLevel(wxCommandEvent &event) {
  const auto selection = event.GetString().ToStdString();
  const auto find = std::ranges::find_if(log_level_list_, [&] (const auto& itr) {
    return itr.second == selection;
  });
  if (find != log_level_list_.cend()) {
    for (auto& client : client_list_) {
      client->SendLogLevel(find->first);
    }
  }
}

void MainFrame::OnClientList(wxListEvent& event) {
  const auto name = event.GetText().ToStdString();
  auto find = std::ranges::find_if(client_list_, [&] (const auto& client) {
    return name == client->Name();
  });
  if (find != client_list_.end()) {
    (*find)->Active(!(*find)->Active());
  }
}

bool MainFrame::FilterMessage(const ListenTextMessage& msg) const {
  if (wildcard_.empty()) {
    return false; // Show everything
  }
  return !util::string::WildcardMatch(msg.text_, wildcard_, false) &&
         !util::string::WildcardMatch(msg.pre_text_, wildcard_, false);
}

} // end namespace