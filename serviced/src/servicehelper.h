/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <windows.h>

namespace detail::services {

enum class StartupType : uint8_t {
  Manual = 0,    ///< Only manual start through GUI.
  Disabled = 1,  ///< Disabled service
  Automatic = 2  ///< Always running. Restart at failure.
};

class ServiceHelper final {
 public:

  ServiceHelper() = default;
  ~ServiceHelper() = default;

  void ParentPath(const std::string& parent_path) {
    parent_path_ = parent_path;
  }

  void Name(const std::string& service_name) {
    name_ = service_name;
  }

  [[nodiscard]] const std::string& Name() const {
    return name_;
  }

  [[nodiscard]] DWORD ProcessId() const {
    return process_info_.dwProcessId;
  }

  static ServiceHelper& Instance();
  bool ReadRegistryInfo();
  bool RunService();
  DWORD SvcCtrlHandler(DWORD control, DWORD event_type, void* event_data, void* context);
  void ServiceMain(DWORD nof_arg, char* arg_list[]);

 private:
  std::string name_;
  std::string parent_path_;
  std::string exe_name_;
  std::vector<std::string> exe_arguments_;
  DWORD priority_ = IDLE_PRIORITY_CLASS;
  StartupType startup_ = StartupType::Manual;
  SERVICE_STATUS_HANDLE status_handle_ = nullptr;
  std::atomic<DWORD> state_ = SERVICE_STOPPED;
  std::atomic<DWORD> check_point_ = 0;
  std::atomic<bool> stop_ = false;
  PROCESS_INFORMATION process_info_ {};
  size_t restarts_ = 0;

  void DoSuperviseApp();
  void FromExePath(const std::string& exe_path);
  void ReportStatus();
  bool RegisterService();
  bool StartApp();
  void StopApp();
  void SuperviseApp();

};
} // end namespace




