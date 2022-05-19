/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include <boost/process.hpp>
#include <shellapi.h>

#include <wx/wx.h>
#include <wx/docview.h>

#include <wx/config.h>
#include <wx/utils.h>
#include <wx/splash.h>
#include <wx/stdpaths.h>
#include <wx/cmdline.h>

#include "util/logconfig.h"
#include "util/logstream.h"

#include "serviceexplorerid.h"
#include "serviceexplorer.h"
#include "mainframe.h"


using namespace util::log;

namespace {

const wxCmdLineEntryDesc kCmdLineList[] = {
    { wxCMD_LINE_SWITCH, "h", "help", "Displays help", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_OPTION, "i", "install", "Installs the service", wxCMD_LINE_VAL_STRING, 0  },
    { wxCMD_LINE_OPTION, "u", "uninstall", "Uninstalls the service", wxCMD_LINE_VAL_STRING, 0  },
    { wxCMD_LINE_OPTION, "s", "start", "Installs the service", wxCMD_LINE_VAL_STRING, 0  },
    { wxCMD_LINE_OPTION, "p", "stop", "Installs the service", wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_NONE }
};

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

} // end namespace

namespace util::service::gui  {

wxIMPLEMENT_APP(ServiceExplorer); // NOLINT

wxBEGIN_EVENT_TABLE(ServiceExplorer, wxApp) // NOLINT
  EVT_UPDATE_UI(kIdOpenLogFile,ServiceExplorer::OnUpdateOpenLogFile)
  EVT_MENU(kIdOpenLogFile, ServiceExplorer::OnOpenLogFile)
wxEND_EVENT_TABLE()

bool ServiceExplorer::OnInit() {
  if (!wxApp::OnInit()) {
    return false;
  }

    // Setup system basic configuration
  SetVendorDisplayName("Report Server");
  SetVendorName("ReportServer");
  SetAppName("ServiceExplorer");
  SetAppDisplayName("Service Explorer");

  // Check the command line. If any options, start without a GUI
  if (!CheckCmdLine()) {
    return false; // Normal command line option  return
  }

  auto* app_config = wxConfig::Get();
  wxPoint start_pos;
  app_config->Read("/MainWin/X",&start_pos.x, wxDefaultPosition.x);
  app_config->Read("/MainWin/Y",&start_pos.y, wxDefaultPosition.x);
  wxSize start_size;
  app_config->Read("/MainWin/XWidth",&start_size.x, 1200);
  app_config->Read("/MainWin/YWidth",&start_size.y, 800);

  bool maximized = false;
  app_config->Read("/MainWin/Max",&maximized, maximized);

  auto* frame = new MainFrame(GetAppDisplayName(), start_pos, start_size, maximized);

  wxInitAllImageHandlers();
  wxBitmap splash_bitmap("SPLASH", wxBITMAP_TYPE_PNG_RESOURCE);

  wxSplashScreen splash(splash_bitmap, wxSPLASH_CENTRE_ON_PARENT | wxSPLASH_TIMEOUT,
                                    10000, frame, wxID_ANY);

  splash.Show();

  // Set up the log file.
  // The log file will be in %TEMP%/report_server/mdf_viewer.log
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("report_server");
  log_config.BaseName("service_explorer");
  log_config.CreateDefaultLogger();
  LOG_INFO() << "Log File created. Path: " << log_config.GetLogFile();

  FindNotepad();
  FindServiceDaemon();
  ServiceHelper::Instance().CheckAccessLevels();
  if (!ServiceHelper::Instance().HaveReadAccess()) {
   wxMessageDialog access(frame,
            "This user doesn't any access rights for services.\nApplication will not work properly.",
            "No Access", wxOK | wxCENTRE | wxICON_AUTH_NEEDED );
   access.ShowModal();
  }

  if (ServiceHelper::Instance().HaveControlAccess()) {
    LOG_INFO() << "The user may start and stop services.";
  }

  if (ServiceHelper::Instance().HaveAllAccess()) {
    LOG_INFO() << "The user have full access rights to services.";
  }

  ServiceHelper::Instance().FetchServices();
  frame->RedrawServiceList();
  frame->Show(true);
  SetTopWindow(frame);
  return true;
}

int ServiceExplorer::OnExit() {
  LOG_INFO() << "Closing application";
  auto* doc_manager = wxDocManager::GetDocumentManager();
  delete doc_manager;

  auto& log_config = LogConfig::Instance();
  log_config.DeleteLogChain();
  return wxApp::OnExit();
}

void ServiceExplorer::OnOpenLogFile(wxCommandEvent& event) { //NOLINT
  auto& log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  OpenFile(logfile);
}

void ServiceExplorer::OnUpdateOpenLogFile(wxUpdateUIEvent &event) {
  if (notepad_.empty()) {
    event.Enable(false);
    return;
  }

  auto& log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  try {
    std::filesystem::path p(logfile);
    const bool exist = std::filesystem::exists(p);
    event.Enable(exist);
  } catch (const std::exception&) {
    event.Enable(false);
  }
}


void ServiceExplorer::OpenFile(const std::string& filename) const {
  if (!notepad_.empty()) {
    boost::process::spawn(notepad_, filename);
  }
}

void ServiceExplorer::FindNotepad() {

  // 1. Find the path to the 'notepad++.exe'
  try {
    std::vector< boost::filesystem::path > path_list = ::boost::this_process::path();
    path_list.emplace_back("c:/program files/notepad++");

    auto notepad = boost::process::search_path("notepad++", path_list);
    if (!notepad.string().empty()) {
      notepad_ = notepad.string();
      LOG_INFO() << "Found NotePad++. Path: " << notepad_;
    }
  } catch(const std::exception& ) {
    notepad_.clear();
  }

  // 2. Find the path to the 'notepad.exe'
  if (!notepad_.empty()) {
    return;
  }

  try {
    auto notepad = boost::process::search_path("notepad");
    if (!notepad.string().empty()) {
      notepad_ = notepad.string();
      LOG_INFO() << "Found NotePad. Path: " << notepad_;
    }
  } catch(const std::exception& ) {
    notepad_.clear();
    LOG_INFO() << "NotePad was not found.";
  }
}

void ServiceExplorer::FindServiceDaemon() {

  // 1. On the same path as this executable
  const auto& path_list = wxStandardPaths::Get();
  auto exe_path = path_list.GetExecutablePath();
  try {
    std::filesystem::path serv = exe_path.ToStdWstring();
    serv.replace_filename("serviced.exe");
    if (std::filesystem::exists(serv)) {
      serviced_ = serv.string();
      LOG_INFO() << "Found Service Daemon. Path: " << serviced_;
    }
  } catch(const std::exception& ) {
    serviced_.clear();
  }

  // 2. On the cmake build path
  if (!serviced_.empty()) {
    return;
  }
  try {
    const auto find = exe_path.Replace(L"serviceexplorer",L"serviced",true);
    std::filesystem::path serv = exe_path.ToStdWstring();
    if (find > 0 && std::filesystem::exists(serv)) {
      serviced_ = serv.string();
      LOG_INFO() << "Found Service Daemon. Path: " << serviced_;
    }
  } catch(const std::exception& ) {
    serviced_.clear();
  }

  // 3. Let boost find it
  if (!serviced_.empty()) {
    return;
  }
  try {
    auto serv = boost::process::search_path("serviced");
    if (!serv.string().empty()) {
      serviced_ = serv.string();
      LOG_INFO() << "Found Service Daemon. Path: " << serviced_;
    }
  } catch(const std::exception& ) {
    serviced_.clear();
    LOG_INFO() << "Service Daemon was not found.";
  }
}

void ServiceExplorer::OnInitCmdLine(wxCmdLineParser &parser) {
  parser.EnableLongOptions();
  parser.SetDesc(kCmdLineList);
  parser.SetSwitchChars("-");
}

bool ServiceExplorer::OnCmdLineParsed(wxCmdLineParser &parser) {
  parser.Found("i",&install_);
  parser.Found("u",&uninstall_);
  parser.Found("s",&start_);
  parser.Found("p",&stop_);
  return true;
}

bool ServiceExplorer::CheckCmdLine() {
  bool cmd_line = false;
  if (!install_.empty()) {
    InstallService(install_.ToStdWstring());
    cmd_line = true;
  }

  if (!start_.empty()) {
    StartServiceInt(start_.ToStdWstring());
    cmd_line = true;
  }

  if (!stop_.empty()) {
    StopServiceInt(stop_.ToStdWstring());
    cmd_line = true;
  }

  if (!uninstall_.empty()) {
    UninstallService(uninstall_.ToStdWstring());
    cmd_line = true;
  }

  return !cmd_line; // returning false means to not show the GUI
}


void ServiceExplorer::RunAs(HWND hwnd, const std::wstring& params) const {
  const auto& path_list = wxStandardPaths::Get();
  const auto exe_path = path_list.GetExecutablePath();
  const std::wstring& command = exe_path.ToStdWstring();
  auto* main_window = GetTopWindow();

  SHELLEXECUTEINFOW info {};
  info.cbSize = sizeof(info);
  info.fMask = SEE_MASK_NOCLOSEPROCESS ;
  info.hwnd = hwnd;
  info.lpVerb = L"runas";
  info.lpFile = command.c_str();
  info.lpParameters = params.c_str();
  info.lpDirectory = nullptr;
  info.nShow = SW_SHOWNORMAL;

  const auto ret = ::ShellExecuteExW(&info);
  if (!ret) {
    std::ostringstream error;
    error << "Failed to execute as administrator!" << std::endl
          << "Error: " << GetLastErrorAsString();
    wxMessageDialog err(main_window,error.str(),"Error Install Service", wxOK | wxCENTRE | wxICON_ERROR);
    err.ShowModal();
    return;
  }
  ::WaitForSingleObject(info.hProcess, 10'000);
  ::CloseHandle(info.hProcess);
}

} // end namespace


