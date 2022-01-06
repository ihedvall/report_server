/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <sstream>
#include "listenproxy.h"
namespace util::log::detail {

ListenProxy::ListenProxy(const std::string &share_name)
: IListen(),
  queue_(false, share_name) {

}

ListenProxy::~ListenProxy() {
  queue_.Stop();
}

bool ListenProxy::IsActive() const {
 return queue_.IsActive();
}

size_t ListenProxy::LogLevel() {
  return queue_.LogLevel();
}

void ListenProxy::AddMessage(uint64_t nano_sec_1970,
                             const std::string &pre_text,
                             const std::string &text) {
  if (!IsActive()) {
    return;
  }

  // Need to adjust pre_text and text so the fit into the shared memory messages
  std::string temp = pre_text;
  while (temp.size() >= sizeof(SharedListenMessage::pre_text)) {
    temp.pop_back();
  }

  if (text.size() < sizeof(SharedListenMessage::text)) {
    SharedListenMessage msg;
    msg.ns1970 = nano_sec_1970;
    strcpy(msg.pre_text, temp.c_str());
    strcpy(msg.text, text.c_str());
    queue_.Add(msg);
    return;
  }

  // Split into more messages
  size_t msg_count;
  size_t count = 0;
  std::ostringstream out;

  for (const char in_char : text) {
    if (count >= sizeof(SharedListenMessage::text) ) {
        // Send message with ugly line wrap
      SharedListenMessage msg;
      msg.ns1970 = msg_count == 0 ? nano_sec_1970 : 0; // Indicate wrapped messages
      strcpy(msg.pre_text, temp.c_str());
      strcpy(msg.text,out.str().c_str());
      queue_.Add(msg);
      ++msg_count;

      out.str("");
      out.clear();
      count = 0;
    } else if (in_char == ' ' && count >= sizeof(SharedListenMessage::text) - 10) {
      // Send message with nice line wrap
      SharedListenMessage msg;
      msg.ns1970 = msg_count == 0 ? nano_sec_1970 : 0; // Indicate wrapped messages
      strcpy(msg.pre_text, temp.c_str());
      strcpy(msg.text,out.str().c_str());
      queue_.Add(msg);
      ++msg_count;

      out.str("");
      out.clear();
      count = 0;
      continue;
    }

    out << in_char;
    ++count;
  }
  if (!out.str().empty()) {
    SharedListenMessage msg;
    msg.ns1970 = msg_count == 0 ? nano_sec_1970 : 0; // Indicate wrapped messages
    strcpy(msg.pre_text, temp.c_str());
    strcpy(msg.text,out.str().c_str());
    queue_.Add(msg);
  }
}


} // end namespace

