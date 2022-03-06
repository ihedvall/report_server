/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <memory>
#include <boost/circular_buffer.hpp>
#include <wx/listctrl.h>
#include "../../utillib/src/listenmessage.h"

namespace util::log::gui {

using MessagePtr = std::unique_ptr<util::log::detail::ListenMessage>;
using MessageBuffer = boost::circular_buffer<MessagePtr>;
class ListenListView : public wxListView {
 public:
  ListenListView(wxWindow* parent, MessageBuffer& buffer);

  void ShowDate(bool show_date) {
    show_date_ = show_date;
  }

  [[nodiscard]] bool ShowDate() const {
    return show_date_;
  }

  void ShowMicroSeconds(bool show_us) {
    show_us_ = show_us;
  }

  [[nodiscard]] bool ShowMicroSeconds() const {
    return show_us_;
  }

 protected:
  [[nodiscard]] wxString OnGetItemText(long item, long column) const override;
 private:
  MessageBuffer& buffer_;
  bool show_us_ = false;
  bool show_date_ = false;

  [[nodiscard]] std::string FormatTime(uint64_t nsSince70) const;
};


} // end namespace


