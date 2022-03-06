/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <sstream>
#include "listenlistview.h"
#include "listenviewerid.h"
#include "util/timestamp.h"
using namespace util::log::detail;
namespace util::log::gui {

ListenListView::ListenListView(wxWindow *parent, MessageBuffer& buffer)
: wxListView(parent, kIdMessageList, wxDefaultPosition, {1000, 700}, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL),
  buffer_(buffer) {

}

wxString ListenListView::OnGetItemText(long item, long column) const {
  if (item < 0 || item >= buffer_.size()) {
    return {};
  }
  const auto& msg = buffer_[item];
  if (!msg) {
    return {};
  }
  const auto* text_msg = dynamic_cast<const ListenTextMessage*>(msg.get());
  if (text_msg == nullptr) {
    return {};
  }

  switch (column) {
    case 0:
      return text_msg->ns1970_ > 0 ? FormatTime(text_msg->ns1970_) : std::string();

    case 1:
      return text_msg->ns1970_ > 0 ? text_msg->pre_text_ : std::string();

    case 2:
      return text_msg->text_;

    default:
      break;
  }
  return wxListCtrlBase::OnGetItemText(item, column);
}

std::string ListenListView::FormatTime(uint64_t nsSince70) const {
  std::ostringstream temp;
  if (ShowDate()) {
    temp << util::time::NsToLocalDate(nsSince70) << " ";
  }
  temp << util::time::NsToLocalTime(nsSince70, ShowMicroSeconds() ? 2 : 1);
  return temp.str();
}

} // end namespace