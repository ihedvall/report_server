/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <thread>
#include <chrono>
#include "util/logstream.h"
#include "servicehelper.h"

using namespace util::log;
using namespace std::chrono_literals;

namespace {

std::string GetLastErrorAsString() {
  auto errorMessageID = ::GetLastError();
  if(errorMessageID == 0) {
    return {};
  }

  LPSTR messageBuffer = nullptr;
  size_t size = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr, errorMessageID,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPSTR) &messageBuffer, 0, nullptr);
  std::string message(messageBuffer, size);
  LocalFree(messageBuffer);
  return message;
}

void Start(SC_HANDLE service_manager, const std::wstring& name) {

  auto* service = ::OpenServiceW(
      service_manager,              // SCM database
      name.c_str(),          // name of service
      GENERIC_EXECUTE | GENERIC_READ);

  if (service == nullptr) {
    std::ostringstream error;
    error << "Failed to open the service!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " <<  GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Start Service", wxOK | wxCENTRE | wxICON_ERROR);
    err.ShowModal();
    return;
  }

  const auto start = ::StartServiceW(service,0, nullptr);

  if (!start) {
    ::CloseServiceHandle(service);
    std::ostringstream error;
    error << "Failed to start the service!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " <<  GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Start Service", wxOK | wxCENTRE | wxICON_WARNING);
    err.ShowModal();
    return;
  }
  bool timeout = true;
  {
    wxBusyCursor wait;
    for (size_t count = 0; count < 100; ++count) {
      SERVICE_STATUS status{};
      const auto stat = QueryServiceStatus(service, &status);
      if (!stat) {
        break;
      }
      if (status.dwCurrentState == SERVICE_RUNNING) {
        timeout = false;
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
  }
  ::CloseServiceHandle(service);
  if (timeout) {
    std::ostringstream error;
    error << "Failed to start the service!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " <<  "Timeout (10s)";
    wxMessageDialog err(nullptr,error.str(),"Error Start Service", wxOK | wxCENTRE | wxICON_WARNING);
    err.ShowModal();
  }
}

void Stop(SC_HANDLE service_manager, const std::wstring& name) {

  auto *service = ::OpenServiceW(
      service_manager,              // SCM database
      name.c_str(),          // name of service
      GENERIC_EXECUTE | GENERIC_READ);

  if (service == nullptr) {
    std::ostringstream error;
    error << "Failed to open the service!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " << GetLastErrorAsString();
    wxMessageDialog err(nullptr, error.str(), "Error Stop Service", wxOK | wxCENTRE | wxICON_ERROR);
    err.ShowModal();
    return;
  }

  SERVICE_STATUS status1 {};
  const auto stop = ::ControlService(service, SERVICE_CONTROL_STOP, &status1);

  if (!stop) {
    ::CloseServiceHandle(service);
    std::ostringstream error;
    error << "Failed to stop the service!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " << GetLastErrorAsString();
    wxMessageDialog err(nullptr, error.str(), "Error Stop Service", wxOK | wxCENTRE | wxICON_WARNING);
    err.ShowModal();
    return;
  }
  bool timeout = true;
  {
    wxBusyCursor wait;
    for (size_t count = 0; count < 100; ++count) {
      SERVICE_STATUS status{};
      const auto stat = QueryServiceStatus(service, &status);
      if (!stat) {
        break;
      }
      if (status.dwCurrentState == SERVICE_STOPPED) {
        timeout = false;
        break;
      }
      std::this_thread::sleep_for(100ms);
    }
  }
  ::CloseServiceHandle(service);
  if (timeout) {
    std::ostringstream error;
    error << "Failed to stop the service!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " << "Timeout (10s)";
    wxMessageDialog err(nullptr, error.str(), "Error Stop Service", wxOK | wxCENTRE | wxICON_WARNING);
    err.ShowModal();
  }
}

void Install(SC_HANDLE service_manager, const std::wstring& name) {
  util::service::gui::ServiceInfo info;
  info.GetConfig(L"Install");
  info.SaveToRegistry();
  if (name != info.name.ToStdWstring()) {
    std::wostringstream error;
    error << L"Failed to install the service!" << std::endl
          << L"Invalid service name. Name: " << info.name << std::endl
          << L"Service: " << name;
    wxMessageDialog err(nullptr,error.str(),L"Error Install Service", wxOK | wxCENTRE | wxICON_ERROR);
    err.ShowModal();
    return;
  }

  const auto* dependency = info.dependency.empty() ? nullptr : info.dependency.wc_str();
  DWORD startup = 0;
  switch (info.startup) {
    case util::service::gui::StartupType::Automatic:
      startup = SERVICE_AUTO_START;
      break;

    case util::service::gui::StartupType::Disabled:
      startup = SERVICE_DISABLED;
      break;

    case util::service::gui::StartupType::Manual:
    default:
      startup = SERVICE_DEMAND_START;
      break;
  }
  auto* service = ::CreateServiceW(
      service_manager,                           // SCM database
      info.name.ToStdWstring().c_str(),          // name of service
      info.display_name.ToStdWstring().c_str(),  // service name to display
      SERVICE_ALL_ACCESS,        // desired access
      SERVICE_WIN32_OWN_PROCESS, // service type
      startup,      // start type
      SERVICE_ERROR_NORMAL,      // error control type
      info.ToBinaryPath().wc_str(),       // path to service's binary
      nullptr,                      // no load ordering group
      nullptr,                      // no tag identifier
      dependency,                   // no dependencies
      nullptr,                      // LocalSystem account
      nullptr);                     // no password

  if (service == nullptr) {
    std::wostringstream error;
    error << L"Failed to create the service!" << std::endl
          << L"Service: " << name << std::endl
          << L"Error: " <<  GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),L"Error Install Service", wxOK | wxCENTRE | wxICON_ERROR);
    err.ShowModal();
    return;
  }

  SERVICE_DELAYED_AUTO_START_INFO delay = {TRUE};
  const auto change_delay = ::ChangeServiceConfig2W(service, SERVICE_CONFIG_DELAYED_AUTO_START_INFO,&delay);
  if (!change_delay) {
    std::ostringstream error;
    error << "Failed to set the delayed startup!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " <<  GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Install Service", wxOK | wxCENTRE | wxICON_WARNING);
    err.ShowModal();
  }

  std::wstring desc = info.description.ToStdWstring();
  SERVICE_DESCRIPTIONW description = {desc.data()};
  const auto change_description = ::ChangeServiceConfig2W(service, SERVICE_CONFIG_DESCRIPTION,&description);
  if (!change_description) {
    std::ostringstream error;
    error << "Failed to set the delayed startup!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " <<  GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Install Service", wxOK | wxCENTRE | wxICON_WARNING);
    err.ShowModal();
  }

  ::CloseServiceHandle(service);
  {
    std::ostringstream success;
    success << "Installed service successfully." << std::endl
            << "Service: " << name;
    wxMessageDialog message(nullptr, success.str(), "Install Service", wxOK | wxCENTRE | wxICON_INFORMATION);
    message.ShowModal();
  }
}

void Uninstall(SC_HANDLE service_manager, const std::wstring& name) {
  auto* service = ::OpenServiceW(
      service_manager,              // SCM database
      name.c_str(),          // name of service
      SERVICE_ALL_ACCESS);

  if (service == nullptr) {
    std::ostringstream error;
    error << "Failed to open the service!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " <<  GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Uninstall Service", wxOK | wxCENTRE | wxICON_ERROR);
    err.ShowModal();
    return;
  }

  const auto del = ::DeleteService(service);
  if (!del) {
    std::ostringstream error;
    error << "Failed to set the delete the service!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " <<  GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Uninstall Service", wxOK | wxCENTRE | wxICON_WARNING);
    err.ShowModal();
  }
  ::CloseServiceHandle(service);
  {
    std::ostringstream success;
    success << "Uninstalled service successfully." << std::endl
            << "Service: " << name;
    wxMessageDialog message(nullptr, success.str(), "Install Service", wxOK | wxCENTRE | wxICON_INFORMATION);
    message.ShowModal();
  }
}

} // end namespace

namespace util::service::gui  {

void ServiceHelper::FetchServices() {
  wxBusyCursor wait;
  service_list_.clear();
  auto service_manager = OpenSCManagerW(nullptr,nullptr, GENERIC_READ);
  if (service_manager == nullptr) {
    LOG_ERROR() << "Failed to open the service manager. Error: " << GetLastErrorAsString();
    return;
  }
  FetchServiceList(service_manager);
  UpdateServiceList(service_manager);
  const auto close = CloseServiceHandle(service_manager);
  if (!close) {
    LOG_ERROR() << "Failed to close the service manager. Error: " << GetLastErrorAsString();
    return;
  }
}

void ServiceHelper::FetchServiceList(SC_HANDLE service_manager) {
  DWORD bytes_needed = 0;
  DWORD services_returned = 0;
  DWORD resume_handle = 0;
  EnumServicesStatusExW(service_manager,SC_ENUM_PROCESS_INFO,
                        SERVICE_WIN32 , SERVICE_STATE_ALL, nullptr, 0,
                        &bytes_needed, &services_returned, &resume_handle, nullptr);
  if (bytes_needed == 0) {
    LOG_ERROR() << "Failed to get the service list size. Error: " << GetLastErrorAsString();
    return;
  }
  std::vector<uint8_t> temp(bytes_needed, 0);

  const auto list = EnumServicesStatusExW(service_manager,SC_ENUM_PROCESS_INFO,
                                          SERVICE_WIN32 , SERVICE_STATE_ALL, temp.data(), temp.size(),
                                          &bytes_needed, &services_returned, &resume_handle, nullptr);
  if (!list) {
    LOG_ERROR() << "Failed to get the service list. Error: " << GetLastErrorAsString();
    return;
  }

  const auto* services = reinterpret_cast<const ENUM_SERVICE_STATUS_PROCESSW*>(temp.data());
  for (DWORD service = 0; service < services_returned; ++service) {
    const auto& serv = services[service];
    ServiceInfo info;
    info.name = serv.lpServiceName;
    info.display_name = serv.lpDisplayName;
    info.status = serv.ServiceStatusProcess;
    service_list_.insert({info.name.ToStdWstring(), info});
  }
}

void ServiceHelper::UpdateServiceList(SC_HANDLE service_manager) {
  for (auto& itr : service_list_) {
    const auto& name = itr.first;
    auto& info = itr.second;
    auto service = OpenServiceW(service_manager,name.c_str(),GENERIC_READ);
    if (service == nullptr) {
      continue;
    }
    info.UpdateServiceInfo(service);
    CloseServiceHandle(service);
  }
}

void InstallService(const std::wstring& name) {
  auto service_manager = OpenSCManagerW(nullptr,nullptr, GENERIC_ALL);
  if (service_manager == nullptr) {
    std::wostringstream error;
    error << L"Failed to open the service manager!" << std::endl
          << L"Service: " << name << std::endl
          << "Error: " << GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),L"Error Install Service", wxOK | wxCENTRE | wxICON_AUTH_NEEDED);
    err.ShowModal();
    return;
  }

  Install(service_manager, name);
  ::CloseServiceHandle(service_manager);
}


void UninstallService(const std::wstring& name) {
  auto service_manager = OpenSCManagerW(nullptr,nullptr, GENERIC_ALL);
  if (service_manager == nullptr) {
    std::ostringstream error;
    error << "Failed to open the service manager!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " << GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Uninstall Service", wxOK | wxCENTRE | wxICON_AUTH_NEEDED);
    err.ShowModal();
    return;
  }

  Uninstall(service_manager,name);
  ::CloseServiceHandle(service_manager);
}

void StartServiceInt(const std::wstring& name) {
  auto service_manager = OpenSCManagerW(nullptr,nullptr, GENERIC_EXECUTE | GENERIC_READ);
  if (service_manager == nullptr) {
    std::ostringstream error;
    error << "Failed to open the service manager!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " << GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Start Service", wxOK | wxCENTRE | wxICON_AUTH_NEEDED);
    err.ShowModal();
    return;
  }

  Start(service_manager,name);
  ::CloseServiceHandle(service_manager);
}

void StopServiceInt(const std::wstring &name) {
  auto service_manager = OpenSCManagerW(nullptr,nullptr, GENERIC_EXECUTE | GENERIC_READ);
  if (service_manager == nullptr) {
    std::ostringstream error;
    error << "Failed to open the service manager!" << std::endl
          << "Service: " << name << std::endl
          << "Error: " << GetLastErrorAsString();
    wxMessageDialog err(nullptr,error.str(),"Error Start Service", wxOK | wxCENTRE | wxICON_AUTH_NEEDED);
    err.ShowModal();
    return;
  }

  Stop(service_manager,name);
  ::CloseServiceHandle(service_manager);

}

ServiceHelper &ServiceHelper::Instance() {
  static ServiceHelper instance;
  return instance;
}

void ServiceHelper::CheckAccessLevels() {

  {
    auto service_manager = OpenSCManagerW(nullptr, nullptr, GENERIC_ALL);
    if (service_manager == nullptr) {
      access_all_ = false;
    } else {
      access_all_ = true;
      CloseServiceHandle(service_manager);
    }
  }

  {
    auto service_manager = OpenSCManagerW(nullptr, nullptr, GENERIC_READ);
    if (service_manager == nullptr) {
      access_read_ = false;
    } else {
      access_read_ = true;
      CloseServiceHandle(service_manager);
    }
  }

  {
    auto service_manager = OpenSCManagerW(nullptr, nullptr, GENERIC_EXECUTE | GENERIC_READ);
    if (service_manager == nullptr) {
      access_control_ = false;
    } else {
      access_control_ = true;
      CloseServiceHandle(service_manager);
    }
  }
}

}