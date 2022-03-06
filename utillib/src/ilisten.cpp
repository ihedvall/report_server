/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <iomanip>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include "util/stringutil.h"
#include "util/timestamp.h"
#include "util/ilisten.h"
#include "listenserver.h"
#include "listenproxy.h"
namespace util::log {

std::unique_ptr<IListen> CreateListen(const std::string &type, const std::string &share_name) {
  std::unique_ptr<IListen> listen;
  if (util::string::IEquals(type, "ListenProxy") && !share_name.empty()) {
    listen = std::make_unique<detail::ListenProxy>(share_name);
  } else if (util::string::IEquals(type, "ListenServer")) {
    listen = std::make_unique<detail::ListenServer>(share_name);
  }
  return listen;
}

std::string IListen::ParseHex(const std::vector<uint8_t>& buffer) {
  std::ostringstream temp;
  for (uint8_t byte : buffer) {
    if (!temp.str().empty()) {
      temp << " ";
    }
    temp << std::setw(2) << std::setfill('0') << std::hex << static_cast<uint16_t>(byte);
  }
  return temp.str();
}

std::string IListen::Name() const {
  return name_;
}

void IListen::Name(const std::string& name) {
  name_ = name;
}

std::string IListen::Description() const {
  return description_;
}

void IListen::Description(const std::string& description) {
  description_ = description;
}

std::string IListen::PreText() const {
  return pre_text_;
}

void IListen::PreText(const std::string& pre_text) {
  pre_text_ = pre_text;
}

std::string IListen::HostName() const {
  return host_name_;
}

void IListen::HostName(const std::string &host_name) {
  host_name_ = host_name;
}

uint16_t IListen::Port() const {
  return port_;
}

void IListen::Port(uint16_t port) {
  port_ = port;
}

void IListen::SetLogLevelText(size_t level, const std::string &menu_text) {
  log_level_list_.insert({level, menu_text});
}

const std::map<uint64_t, std::string> &IListen::LogLevelList() const {
  return log_level_list_;
}

void IListen::ListenText(const char* format_text, ...) {
  if (!IsActive()) {
    return;
  }

  std::string temp(2000,'\0');
  va_list lsArg {};
  va_start( lsArg, format_text);
  vsnprintf( temp.data(), temp.size(), format_text, lsArg );
  temp.back() = '\0';
  va_end( lsArg );
  std::string trimmed = temp.c_str(); //NOLINT
  AddMessage(time::TimeStampToNs(), pre_text_, trimmed);
}

void IListen::ListenTextEx(uint64_t ns1970, const std::string &pre_text, const char* format_text, ...) {
  if (!IsActive()) {
    return;
  }

  std::string temp(2000,'0');
  va_list lsArg {};
  va_start( lsArg, format_text);
  vsnprintf( temp.data(), temp.size(), format_text, lsArg );
  temp.back() = '\0';
  va_end( lsArg );
  std::string trimmed = temp.c_str(); //NOLINT
  AddMessage(ns1970, pre_text, trimmed);
}

void IListen::ListenTransmit(uint64_t ns1970, const std::string &pre_text, const std::vector<uint8_t> &buffer, void *) {
  if (!IsActive()) {
    return;
  }
  const std::string hex = ParseHex(buffer);
  AddMessage(ns1970, pre_text, hex);
}

void IListen::ListenReceive(uint64_t ns1970, const std::string &pre_text, const std::vector<uint8_t> &buffer, void *hint) {
  if (!IsActive()) {
    return;
  }
  const std::string hex = ParseHex(buffer);
  AddMessage(ns1970, pre_text, hex);
}

bool IListen::Start() {
  return true;
}

bool IListen::Stop() {
  return true;
}


} // end namespace util::log