/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <vector>
#include <filesystem>
#include <atomic>
#include <csignal>
#include <thread>
#include <chrono>
#include <windows.h>

#include <util/logconfig.h>
#include <util/logstream.h>
#include <util/listenconfig.h>
#include <util/ixmlfile.h>
#include <util/ilisten.h>

using namespace util::log;
using namespace util::xml;
using namespace std::chrono_literals;
namespace {
  std::atomic<bool> kStopMain = false;
  std::vector<std::unique_ptr<IListen>> kServerList;
  uint16_t kFreePort = 49152;

  void StopMainHandler(int signal) {
    kStopMain = true;
    LOG_DEBUG() << "Stopping. Signal: " << signal;
    for (size_t count = 0; count < 100 && kStopMain; ++count) {
      std::this_thread::sleep_for(100ms);
    }
  }

  void AddAllKnownServers() {

    // Add System Logger
    {
      auto system_log = util::log::CreateListen("ListenServer", "LISLOG");
      system_log->Name("System Messages");
      system_log->Description("Logs all system messages");
      system_log->HostName("127.0.0.1");
      system_log->Port(kFreePort++);
      system_log->SetLogLevelText(0, "Show all log messages");
      system_log->SetLogLevelText(1, "Hide trace messages");
      system_log->SetLogLevelText(2, "Hide trace/debug messages");
      system_log->SetLogLevelText(3, "Hide trace/debug/info messages");
      kServerList.push_back(std::move(system_log));
    }

    // Add SQLite Logger
    {
      auto sqlite_log = util::log::CreateListen("ListenServer", "LISSQLITE");
      sqlite_log->Name("SQLite messages");
      sqlite_log->Description("Logs all SQL calls");
      sqlite_log->HostName("127.0.0.1");
      sqlite_log->Port(kFreePort++);
      sqlite_log->SetLogLevelText(0, "Show all SQL calls");
      kServerList.push_back(std::move(sqlite_log));
    }
  }
}

int main(int nof_arg, char *arg_list[]) {
  signal(SIGTERM, StopMainHandler);
  signal(SIGABRT, StopMainHandler);
  signal(SIGABRT_COMPAT, StopMainHandler);
  signal(SIGBREAK, StopMainHandler);

  // Set log file name to the service name
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("report_server/log");
  log_config.BaseName("listend");
  log_config.CreateDefaultLogger();
  LOG_DEBUG() << "Log File created. Path: " << log_config.GetLogFile();

  std::vector<std::string> config_files;
  std::string program_data_path = ProgramDataPath();
  for (int arg = 1; arg < nof_arg; ++arg) {
    // configuration files
    const std::string config_file(arg_list[arg]);
    try {
      std::filesystem::path full_name(config_file);
      if (!full_name.has_parent_path()) {
        std::filesystem::path temp_name = program_data_path;
        temp_name.append("report_server");
        temp_name.append("conf");
        temp_name.append(config_file);
        full_name = temp_name;
      }
      if (!std::filesystem::exists(full_name)) {
        LOG_ERROR() << "Config file doesn't exist- File: " << full_name.string();
        continue;
      }
      config_files.emplace_back(full_name.string());
    } catch (const std::exception &error) {
      LOG_ERROR() << "Failed to find the configuration file. File: " << config_file
                  << ", Error: " << error.what();
    }
  }
  // First check if the 'listend.xml' exist in default program path
  if (config_files.empty()) {

    try {
      std::filesystem::path temp_name = program_data_path;
      temp_name.append("report_server");
      temp_name.append("conf");
      temp_name.append("listend.xml");
      if (std::filesystem::exists(temp_name)) {
        config_files.emplace_back(temp_name.string());
      } else {
        LOG_INFO() << "There is no configuration files for this application";
      }
    } catch (const std::exception &error) {
      LOG_ERROR() << "Failed to find the default configuration file. Error: " << error.what();
    }
  }

  // If no configuration files exist. Add all known servers.
  if (config_files.empty()) {
    AddAllKnownServers();
  }

  {
    // Start the listen config
    ListenConfig master;

    // Read all configuration files and
    for (const auto &config_file: config_files) {
      auto xml_file = util::xml::CreateXmlFile();
      xml_file->FileName(config_file);
      const auto parse = xml_file->ParseFile();
      if (!parse) {
        LOG_ERROR() << "Failed to parse the XML file. File: " << config_file;
        continue;
      }
      IXmlNode::ChildList node_list;
      xml_file->GetChildList(node_list);
      for (const auto* node : node_list) {
        if (node->IsTagName("ListenServer")) {
          const auto share_name = node->Property<std::string>("ShareName", "");
          const auto name = node->Property<std::string>("Name", "");
          const auto description = node->Property<std::string>("Description", "");
          const auto pre_text = node->Property<std::string>("PreText", "");
          const auto host_name = node->Property<std::string>("HostName", "127.0.0.1");
          const auto port = node->Property<uint16_t>("Port", 0);

          auto listen = CreateListen("ListenServer", share_name);
          if (!listen) {
            continue;
          }
          listen->Name(name);
          listen->Description(description);
          listen->PreText(pre_text);
          listen->HostName(host_name);
          listen->Port(port);

          const auto* list = node->GetNode("LogLevelList");
          IXmlNode::ChildList log_list;
          if (list != nullptr) {
            list->GetChildList(log_list);
          }
          for (const auto* level : log_list) {
            if (!level->IsTagName("LogLevel")) {
              continue;
            }
            const auto number = level->Attribute<uint64_t>("level", 0);
            const auto menu_text = level->Value<std::string>();
            listen->SetLogLevelText(number, menu_text);
          }
          kServerList.push_back(std::move(listen));
         }
      }
    }
    log_config.AddLogger("Listen Daemon Message", LogType::LogToListen);
    for (auto& server : kServerList) {
      if (!server) {
        continue;
      }
      LOG_DEBUG() << "Starting listen server. Name: " << server->Name();
      server->Start();
    }

    while (!kStopMain) {
      std::this_thread::sleep_for(1000ms);
      LOG_DEBUG() << "Test Message";
    }


    for (auto& server : kServerList) {
      if (!server) {
        continue;
      }
      LOG_DEBUG() << "Stopping listen server. Name: " << server->Name();
      server->Stop();
    }
    log_config.DeleteLogger("Listen Daemon Message");

    kServerList.clear();
    LOG_DEBUG() << "Stopped listen daemon";
  }
  log_config.DeleteLogChain();
  kStopMain = false; // Tells the

  return EXIT_SUCCESS;
}
