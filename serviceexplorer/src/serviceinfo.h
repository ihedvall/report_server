/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/string.h> //NOLINT
#include <wx/wx.h>

namespace util::service::gui {

enum class StartupType : uint8_t {
  Manual = 0,    ///< Only manual start through GUI.
  Disabled = 1,  ///< Disabled service
  Automatic = 2  ///< Always running. Restart at failure.
};

class ServiceInfo {
 public:
  wxString name; ///< Key name of the service (keep short and unique).
  wxString display_name; ///< Display name of the service.
  wxString description; ///< Service description.
  wxString app_path; ///< Service executable path (without file name).
  wxString app_name; ///< Service executable file name (without path).
  std::vector<wxString> arguments; ///< Input arguments to the service.
  wxString dependency; ///< Dependent service. Use key names.
  wxString start_name; ///< Start account (normally local account)
  SERVICE_STATUS_PROCESS status{}; ///< Last status of the service.

  wxString exe_name; ///< Executable to supervise with no or full path. No path means same as serviced.
  std::vector<wxString> exe_arguments; ///< List of input arguments. Normally a configuration file.
  DWORD priority = IDLE_PRIORITY_CLASS; ///< Priority of the executable.
  StartupType startup = StartupType::Automatic; ///< Defines how  the executable should be started.

  [[nodiscard]] bool IsRunning() const;
  [[nodiscard]] bool IsDaemon() const;

  void UpdateServiceInfo(SC_HANDLE service);
  void SaveConfig(const wxString& root_key) const;
  void GetConfig(const wxString& root_key);

  [[nodiscard]] wxString GetPriorityString() const;
  [[nodiscard]] wxString GetTypeString() const;

  void FromBinaryPath(const wxString& binary_path);
  [[nodiscard]] wxString ToBinaryPath()  const;

  void FromExePath(const wxString& exe_path);
  [[nodiscard]] wxString ToExePath()  const;

  void GetFromRegistry();
  void SaveToRegistry();
};

} // end namespace



