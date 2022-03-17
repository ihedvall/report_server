/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <boost/circular_buffer.hpp>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../../utillib/src/listenclient.h"
#include "../../utillib/src/listenmessage.h"
#include "listenlistview.h"

namespace util::log::gui {
class MainFrame : public wxFrame {
 public:
  MainFrame(const wxString& title, const wxPoint& start_pos, const wxSize& start_size, bool maximized);
  void RedrawMessageList();
  void RedrawActiveList();
 private:
  ListenListView* message_list_ = nullptr;
  wxListView* active_list_ = nullptr;
  wxImageList image_list_;
  wxChoice* log_level_ = nullptr;
  wxTimer main_timer_;

  size_t max_lines_ = 10'000;
  size_t wrap_around_ = 120;

  bool auto_scroll_ = true;
  std::map<uint64_t, std::string> log_level_list_;
  std::string wildcard_;

  using ListenClientList = std::vector<std::unique_ptr<util::log::detail::ListenClient>>;
  ListenClientList client_list_;

  MessageBuffer message_buffer_;

  void HandleMessages(util::log::detail::ListenClient& client);
  void HandleStatusBitmap(const util::log::detail::ListenClient& client);
  [[nodiscard]] bool FilterMessage(const util::log::detail::ListenTextMessage& msg) const;

  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnSetting(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnAutoScroll(wxCommandEvent& event);
  void OnClearList(wxCommandEvent& event);
  void OnMainTimer(wxTimerEvent& event);
  void OnSizeChange(wxSizeEvent& event);
  void OnWildcardChange(wxCommandEvent& event);
  void OnWildcardReset(wxCommandEvent& event);
  void OnLogLevel(wxCommandEvent& event);
  void OnClientList(wxListEvent& event);
wxDECLARE_EVENT_TABLE();
};

} // end namespace

