/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <map>
#include "util/stringutil.h"
#include "serviceinfo.h"
namespace util::service::gui {

void InstallService(const std::wstring& name);
void UninstallService(const std::wstring& name);
void StartServiceInt(const std::wstring& name);
void StopServiceInt(const std::wstring& name);

class ServiceHelper {
 public:
  using ServiceList = std::map<std::wstring, ServiceInfo, util::string::IgnoreCase>;

  [[nodiscard]] const ServiceList &GetServiceList() const {
    return service_list_;
  }
  static ServiceHelper& Instance();
  void CheckAccessLevels();
  void FetchServices();

  [[nodiscard]] bool HaveReadAccess() const {
    return access_read_;
  }
  [[nodiscard]] bool HaveControlAccess() const {
    return access_control_;
  }
  [[nodiscard]] bool HaveAllAccess() const {
    return access_all_;
  }

 private:
  bool access_read_ = false;
  bool access_all_ = false;
  bool access_control_ = false;
  ServiceList service_list_;
  void FetchServiceList(SC_HANDLE service_manager);
  void UpdateServiceList(SC_HANDLE service_manager);

};

} // end namespace


