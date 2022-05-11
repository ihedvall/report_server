/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <windows.h>
#include <boost/program_options.hpp>
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

std::string GetLastErrorAsString(const LSTATUS& error) {
  LPSTR messageBuffer = nullptr;
  size_t size = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr, error,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPSTR) &messageBuffer, 0, nullptr);
  std::string message(messageBuffer, size);
  LocalFree(messageBuffer);
  return message;
}

DWORD WINAPI SvcCtrlHandler(DWORD control, DWORD event_type, void* event_data, void* context) {
  // Handle the requested control code.
  auto &service = detail::services::ServiceHelper::Instance();
  return service.SvcCtrlHandler(control, event_type, event_data, context);
}


void WINAPI ServiceMain(DWORD nof_arg, char* arg_list[]) {
  auto &service = detail::services::ServiceHelper::Instance();
  service.ServiceMain(nof_arg, arg_list);
}

BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam ) {
  const auto* service = reinterpret_cast< detail::services::ServiceHelper* > ( lParam );
  if ( service == nullptr ) {
    return FALSE;
  }

  DWORD dProcessId = 0;
  ::GetWindowThreadProcessId( hwnd, &dProcessId );
  if ( dProcessId == service->ProcessId() ) {
    ::PostMessage( hwnd, WM_CLOSE, 0, 0 );
    ::PostMessage( hwnd, WM_CLOSE, 0, 0 );
    ::PostMessage( hwnd, WM_QUIT, 0, 0 );
    ::PostMessage( hwnd, WM_QUIT, 0, 0 );
  }
  return TRUE;
}

} // end namespace

namespace detail::services {

ServiceHelper &ServiceHelper::Instance() {
  static ServiceHelper instance;
  return instance;
}

bool ServiceHelper::ReadRegistryInfo() {
  if (name_.empty()) {
    LOG_ERROR() << "No service name (empty). Invalid use of function";
    return false;
  }

  HKEY reg_key = nullptr;
  std::ostringstream temp;
  temp << R"(Software\ReportServer\Services\)" << name_;
  const auto open = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE,temp.str().c_str(), 0, KEY_READ, &reg_key);
  if (open != ERROR_SUCCESS) {
    LOG_ERROR() << "Failure to open registry key. Key (HKLM): " << temp.str()
      << ". Error: " << GetLastErrorAsString(open);
    return false;
  }
  {
    DWORD size = 0;
    DWORD type = REG_SZ;
    const auto get_size = ::RegQueryValueExA(reg_key, "ExePath", nullptr, &type, nullptr, &size);
    if (get_size == ERROR_SUCCESS && size > 0) {
      std::vector<uint8_t> buffer(size, 0);
      const auto get = ::RegQueryValueExA(reg_key, "ExePath", nullptr, &type, buffer.data(), &size);
      if (get == ERROR_SUCCESS) {
        const std::string exe_path = reinterpret_cast<const char *>(buffer.data());
        FromExePath(exe_path);
      }
    } else {
      LOG_ERROR() << "Failure to get registry value (size). Value: " << "ExePath"
                  << ". Error: " << GetLastErrorAsString(get_size);
    }
  }
  {
    DWORD size = sizeof(priority_);
    DWORD type = REG_DWORD;
    const auto get = ::RegQueryValueExA(reg_key, "Priority", nullptr, &type,
                                        reinterpret_cast<LPBYTE>(&priority_), &size);
    if (get != ERROR_SUCCESS) {
      LOG_ERROR() << "Failure to get registry value (size). Value: " << "Priority"
                  << ". Error: " << GetLastErrorAsString(get);
    }
  }
  {
    DWORD size = sizeof(DWORD);
    DWORD type = REG_DWORD;
    DWORD value = 0;
    const auto get = ::RegQueryValueExA(reg_key, "Startup", nullptr, &type,
                                        reinterpret_cast<LPBYTE>(&value), &size);
    if (get != ERROR_SUCCESS) {
      LOG_ERROR() << "Failure to get registry value (size). Value: " << "Startup"
                  << ". Error: " << GetLastErrorAsString(get);
    } else {
      startup_ = static_cast<StartupType>(value);
    }
  }
  ::RegCloseKey(reg_key);
  return true;
}

void ServiceHelper::FromExePath(const std::string& exe_path) {
  exe_arguments_.clear();
  try {
    const auto argument_list = boost::program_options::split_winmain(exe_path);
    for (size_t argc = 0; argc < argument_list.size(); ++argc) {
      if (argc == 0) {
        exe_name_ = std::string(argument_list[0]);
      } else {
        exe_arguments_.emplace_back(argument_list[argc]);
      }
    }
  } catch (const std::exception&) {
    exe_name_.clear();
  }
}

bool ServiceHelper::RunService() {
  const SERVICE_TABLE_ENTRYA dispatch_list[] = {
    { name_.data(), ::ServiceMain },
    { nullptr, nullptr }
  };

  const auto run = StartServiceCtrlDispatcherA(dispatch_list);
  if (!run) {
    LOG_ERROR() << "Failure to run the service. Service: " << name_
                << ". Error: " << GetLastErrorAsString();
    return false;
  }
  return true;
}


void ServiceHelper::ReportStatus() {
  SERVICE_STATUS status = {SERVICE_WIN32_OWN_PROCESS,state_,0,NO_ERROR,NO_ERROR,check_point_,0};
  switch (state_) {
    case SERVICE_START_PENDING:
    case SERVICE_STOP_PENDING:
      status.dwControlsAccepted = 0;
      status.dwWaitHint = 2000;
      ++check_point_;
      break;

    case SERVICE_STOPPED:
      status.dwControlsAccepted = 0;
      check_point_ = 0;
      break;

    case SERVICE_CONTINUE_PENDING:
    case SERVICE_PAUSE_PENDING:
    case SERVICE_PAUSED:
    case SERVICE_RUNNING:
    default:
      status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
      check_point_ = 0;
      break;
  }
  status.dwCheckPoint = check_point_;
  // Report the status of the service to the SCM.
  ::SetServiceStatus( status_handle_, &status );
}

bool ServiceHelper::RegisterService() {
  status_handle_ = ::RegisterServiceCtrlHandlerExA(name_.c_str(),::SvcCtrlHandler,nullptr);
  if (status_handle_ == nullptr) {
    LOG_ERROR() << "Failure to register the control handler. Service: " << name_
                << ". Error: " << GetLastErrorAsString();
    return false;
  }
  return true;
}

DWORD ServiceHelper::SvcCtrlHandler(DWORD control, DWORD , void *, void *) {
  switch(control) {
      case SERVICE_CONTROL_SHUTDOWN:
      case SERVICE_CONTROL_STOP:
      stop_ = true;
      ReportStatus();
      break;

    case SERVICE_CONTROL_INTERROGATE:
      ReportStatus();
      break;

    case SERVICE_CONTROL_CONTINUE:
    case SERVICE_CONTROL_PAUSE:
    case SERVICE_CONTROL_PARAMCHANGE:
    default:
      return ERROR_CALL_NOT_IMPLEMENTED;
  }
  return NO_ERROR;
}

void ServiceHelper::ServiceMain(DWORD nof_arg , char* arg_list[]) {
  for (DWORD arg = 0; arg < nof_arg; ++arg) {
    LOG_DEBUG() << "Command line argument. Arg" << arg << ": " << arg_list[arg];
  }
  const auto reg = RegisterService();
  if (!reg) {
    LOG_ERROR() << "Failed to register the service. Service: " << Name();
    return;
  }
  restarts_ = 0;
  stop_ = false;
  state_ = SERVICE_START_PENDING; // Initial
  ReportStatus();
  while (!stop_)  {
    const DWORD old_state = state_;
    DoSuperviseApp();
    std::this_thread::sleep_for(state_ == SERVICE_RUNNING ? 1000ms : 100ms);
    if (old_state != state_) {
      ReportStatus();
    }
  }
  if (state_ != SERVICE_STOPPED) {
    state_ = SERVICE_STOP_PENDING;
    ReportStatus();
  }
  ReportStatus();
  StopApp();
  state_ = SERVICE_STOPPED;
  ReportStatus();
}

void ServiceHelper::DoSuperviseApp() {
  switch (state_) {
    case SERVICE_START_PENDING: {
      const auto start = StartApp();
      if (start) {
        state_ = SERVICE_RUNNING;
      } else {
        ++restarts_;
        if (restarts_ > 10) {
          LOG_ERROR() << "Service stopped due to many restarts (10x). Service: " << Name();
          stop_ = true;
        }
      }
      break;
    }

    case SERVICE_STOP_PENDING: {
      stop_ = true;
      StopApp();
      state_ = SERVICE_STOPPED;
      break;
    }

    case SERVICE_STOPPED:
      StopApp();
      break;

    case SERVICE_RUNNING:
    default: {
      SuperviseApp();
      break;
    }
  }
}

bool ServiceHelper::StartApp() {
  std::ostringstream app_name;
  if (strchr(exe_name_.c_str(), ' ') != nullptr) {
    app_name << '\"' << exe_name_ << '\"';
  } else {
    app_name << exe_name_;
  }
  const std::string application_name = app_name.str();

  std::ostringstream cli;
  cli << application_name;
  for (const auto& arg : exe_arguments_) {
    cli << " ";
    if (strchr(arg.c_str(), ' ') != nullptr) {
      cli << '\"' << arg << '\"';
    } else {
      cli << arg;
    }
  }
  std::string command_line = cli.str();

  STARTUPINFOA startup_info {};
  startup_info.cb = sizeof(startup_info);
  startup_info.dwFlags = STARTF_USESHOWWINDOW;
  startup_info.wShowWindow = SW_HIDE;

  process_info_ = {};

  const auto create = ::CreateProcessA(application_name.c_str(),command_line.data(), nullptr, nullptr,
                                       FALSE,priority_, nullptr, nullptr, &startup_info, &process_info_ );
  if (create) {
    CloseHandle(process_info_.hThread); // Don't close the process handle. It is used to supervise the process
    LOG_DEBUG() << "The application was started. Command Line: " << command_line;
    LOG_INFO() << "The service was started. Service: " << Name();
  } else {
    LOG_ERROR() << "The application failed to start. Command Line: " << command_line
                << ", Error: " << GetLastErrorAsString();
    return false;
  }
  return true;
}

void ServiceHelper::StopApp() {
  if (process_info_.hProcess == nullptr) {
    return;
  }

  ::PostThreadMessage(process_info_.dwThreadId, WM_QUIT, 0, 0);
  ::EnumWindows( EnumWindowsProc, reinterpret_cast<LPARAM>(this) );

  const DWORD nResult = ::WaitForSingleObject( process_info_.hProcess, 10000 ); // Wait 10 seconds on close/exit
  if ( nResult != WAIT_TIMEOUT ) { // Normal exit
    LOG_DEBUG() << "Stopped service. Name: " << Name();
  } else {
    LOG_ERROR() << "Failed to stop the application normally. Killing the process. Name: " << Name();
    ::TerminateProcess( process_info_.hProcess, 100 );
  }
  ::CloseHandle( process_info_.hProcess );
  process_info_.hProcess = nullptr;
}

void ServiceHelper::SuperviseApp() {
  switch (startup_) {
    case StartupType::Automatic: {
      // Restart if application stopped
      const auto wait = ::WaitForSingleObject(process_info_.hProcess, 0);
      if (wait != WAIT_TIMEOUT) {
        StopApp();
        if (restarts_ > 10) {
          LOG_ERROR() << "Stopped service due to many restarts (10x). Service: " << Name();
          stop_ = true;
        } else {
          std::this_thread::sleep_for(2000ms);
          StartApp();
          ++restarts_;
        }
      }
      break;
    }

    case StartupType::Manual: {
      // Do not restart if application stopped
      const auto wait = ::WaitForSingleObject(process_info_.hProcess, 0);
      if (wait != WAIT_TIMEOUT) {
        StopApp();
        stop_ = true;
      }
      break;
    }

    default: {
        // Always stop
      StopApp();
      stop_ = true;
      break;
    }
  }
}


}