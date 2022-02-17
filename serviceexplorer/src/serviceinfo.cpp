/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <vector>
#include <filesystem>
#include <algorithm>
#include <ranges>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <wx/config.h>
#include "util/logstream.h"
#include "serviceexplorer.h"
#include "serviceinfo.h"

using namespace boost::program_options;
using namespace std::filesystem;
using namespace util::log;

namespace util::service::gui {

void ServiceInfo::UpdateServiceInfo(SC_HANDLE service) {
  DWORD bytes_needed = 0;
  QueryServiceConfigW(service, nullptr, 0, &bytes_needed);
  if (bytes_needed == 0) {
    return;
  }
  std::vector<uint8_t> temp(bytes_needed, 0);

  const auto query = QueryServiceConfigW(service, reinterpret_cast<QUERY_SERVICE_CONFIGW *>(temp.data()),
                                         temp.size(), &bytes_needed);
  if (!query) {
    return;
  }
  const auto *config = reinterpret_cast<const QUERY_SERVICE_CONFIGW *>(temp.data());
  std::wstring binary_path = config->lpBinaryPathName != nullptr ? config->lpBinaryPathName : L"";
  FromBinaryPath(binary_path);


  dependency = config->lpDependencies != nullptr ? config->lpDependencies : L"";
  start_name = config->lpServiceStartName != nullptr ? config->lpServiceStartName : L"";
  switch (config->dwStartType) {
    case SERVICE_AUTO_START:
    case SERVICE_BOOT_START:
    case SERVICE_SYSTEM_START:
      startup = StartupType::Automatic;
      break;

    case SERVICE_DISABLED:
      startup = StartupType::Disabled;
      break;

    case SERVICE_DEMAND_START:
    default:
      startup = StartupType::Manual;
      break;
  }

  bytes_needed = 0;
  QueryServiceConfig2W(service,
  SERVICE_CONFIG_DESCRIPTION, nullptr, 0, &bytes_needed);
  if (bytes_needed == 0) {
    return;
  }

  std::vector<uint8_t> temp2(bytes_needed, 0);

  const auto query2 = QueryServiceConfig2W(service, SERVICE_CONFIG_DESCRIPTION, temp.data(),
                                           temp.size(), &bytes_needed);
  if (!query2) {
    return;
  }
  const auto *desc = reinterpret_cast<const SERVICE_DESCRIPTIONW *>(temp.data());
  description = desc->lpDescription != nullptr ? desc->lpDescription : L"";

  if (IsDaemon()) {
    GetFromRegistry();
  }
}

bool ServiceInfo::IsRunning() const {
  return status.dwCurrentState == SERVICE_RUNNING;
}

bool ServiceInfo::IsDaemon() const {
  return boost::algorithm::iequals(L"serviced.exe",app_name.ToStdWstring());
}

void ServiceInfo::SaveConfig(const wxString &root_key) const {
  auto* config = wxConfig::Get();
  if (config == nullptr) {
    return;
  }

  config->Write(root_key + L"/Name", name);
  config->Write(root_key + L"/DisplayName", display_name);
  config->Write(root_key + L"/Description", description);
  config->Write(root_key + L"/BinaryPath", ToBinaryPath());
  config->Write(root_key + L"/Dependency", dependency);
  config->Write(root_key + L"/ExePath", ToExePath());
  config->Write(root_key + L"/Priority", priority);
  config->Write(root_key + L"/Startup", static_cast<int>(startup));
}

void ServiceInfo::GetConfig(const wxString &root_key) {
  auto* config = wxConfig::Get();
  if (config == nullptr) {
    return;
  }
  name = config->Read(root_key + L"/Name");
  display_name = config->Read(root_key + L"/DisplayName");
  description = config->Read(root_key + L"/Description");
  auto binary_path = config->Read(root_key + L"/BinaryPath");
  FromBinaryPath(binary_path);
  dependency = config->Read(root_key + L"/Dependency");
  auto exe_path = config->Read(root_key + L"/ExePath");
  FromExePath(exe_path);
  priority = config->Read(root_key + L"/Priority", static_cast<DWORD>(IDLE_PRIORITY_CLASS));
  startup = static_cast<StartupType>(config->Read(root_key + L"/Startup", static_cast<int>(StartupType::Automatic)));
}

wxString ServiceInfo::GetPriorityString() const {
  switch (priority) {
    case ABOVE_NORMAL_PRIORITY_CLASS: return L"Above Normal";
    case BELOW_NORMAL_PRIORITY_CLASS: return L"Below Normal";
    case HIGH_PRIORITY_CLASS: return L"High";
    case IDLE_PRIORITY_CLASS: return L"Idle";
    case NORMAL_PRIORITY_CLASS: return L"Normal";
    case REALTIME_PRIORITY_CLASS: return L"Real-Time";
    default:
      break;
  }
  return L"Unknown";
}

wxString ServiceInfo::GetTypeString() const {
  switch (startup) {
    case StartupType::Manual: return L"Manual";
    case StartupType::Disabled: return L"Disabled";
    case StartupType::Automatic: return L"Automatic";
    default:
      break;
  }
  return L"Unknown";
}

void ServiceInfo::FromBinaryPath(const wxString& binary_path) {
  arguments.clear();
  try {
    auto argument_list = split_winmain(binary_path.ToStdWstring());
    for (size_t argc = 0; argc < argument_list.size(); ++argc) {
      if (argc == 0) {
        path full_path(argument_list[argc]);
        app_path = full_path.parent_path().wstring();
        app_name = full_path.filename().wstring();
      } else {
        arguments.emplace_back(argument_list[argc]);
      }
    }
  } catch (const std::exception&) {
    app_name = L"Error";
  }
}

wxString ServiceInfo::ToBinaryPath() const {
  std::wostringstream binary_path;
  try {
    std::filesystem::path filename(app_path.ToStdWstring());
    filename.append(app_name.ToStdWstring());
    const auto f_name = filename.wstring();
    auto spaces = std::ranges::count_if(f_name,isspace);
    if (spaces > 0) {
      binary_path << L"\"" << f_name << L"\"";
    } else {
      binary_path << f_name;
    }
    for (const auto& arg : arguments) {
      spaces = std::ranges::count_if(arg.ToStdWstring(),isspace);
      if (spaces > 0) {
        binary_path << L" \"" << arg << L"\"";
      } else {
        binary_path << L" " << arg;
      }
    }

  } catch (const std::exception&) {
  }
  return binary_path.str();
}

void ServiceInfo::FromExePath(const wxString& exe_path) {
  exe_arguments.clear();
  try {
    auto argument_list = split_winmain(exe_path.ToStdWstring());
    for (size_t argc = 0; argc < argument_list.size(); ++argc) {
      if (argc == 0) {
        exe_name = argument_list[argc];
      } else {
        exe_arguments.emplace_back(argument_list[argc]);
      }
    }
  } catch (const std::exception&) {
    exe_name = L"";
  }
}

wxString ServiceInfo::ToExePath() const {
  std::wostringstream exe_path;
  try {
    auto spaces = std::ranges::count_if(exe_name.ToStdWstring(),isspace);
    if (spaces > 0) {
      exe_path << L"\"" << exe_name << L"\"";
    } else {
      exe_path << exe_name;
    }
    for (const auto& arg : exe_arguments) {
      spaces = std::ranges::count_if(arg.ToStdWstring(),isspace);
      if (spaces > 0) {
        exe_path << L" \"" << arg << L"\"";
      } else {
        exe_path << L" " << arg;
      }
    }

  } catch (const std::exception&) {
  }
  return exe_path.str();
}

void ServiceInfo::GetFromRegistry() {
  std::wostringstream root;
  root << "SOFTWARE\\" << wxGetApp().GetVendorName() << "\\" << L"Services" << "\\" << name;

  wxRegKey reg(wxRegKey::HKLM, root.str());
  if (!reg.Exists()) {
    //reg.Close();
    return;
  }

  const auto open = reg.Open(wxRegKey::Read);
  if (!open) {
    return;
  }

  wxString exe_path;
  reg.QueryRawValue("ExePath", exe_path);
  FromExePath(exe_path);

  long temp = 0;
  reg.QueryValue("Priority", &temp);
  priority = static_cast<DWORD>(temp);

  temp = 0;
  reg.QueryValue("Startup", &temp);
  startup = static_cast<StartupType>(temp);
  reg.Close();
}

void ServiceInfo::SaveToRegistry() {
  std::wostringstream root;
  root << "SOFTWARE\\" << wxGetApp().GetVendorName() << "\\" << L"Services" << "\\" << name;

  wxRegKey reg(wxRegKey::HKLM, root.str());
  const bool create = reg.Create();
  if (!create) {
    return;
  }
  const auto open = reg.Open(wxRegKey::Write);
  if (!open) {
    return;
  }

  wxString exe_path = ToExePath();
  reg.SetValue("ExePath", exe_path);

  long temp = static_cast<long>(priority);
  reg.SetValue("Priority", temp);
  priority = static_cast<DWORD>(temp);

  temp = static_cast<long>(startup);
  reg.SetValue("Startup", temp);

  reg.Close();

}

} //end namespace