/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include <util/logconfig.h>
#include <util/logstream.h>
#include "servicehelper.h"

using namespace detail::services;
using namespace util::log;
int main(int nof_arg, char *arg_list[]) {

  auto& service = ServiceHelper::Instance();

  for (int arg = 0; arg < nof_arg; ++arg) {
    switch (arg) {
      case 0:
        try {
          std::filesystem::path full_name(arg_list[0]);
          service.ParentPath(full_name.parent_path().string());
        } catch (const std::exception&) {
        }
        break;

      case 1:
        service.Name(arg_list[1]);
        break;

      default:
        break;
    }
  }

  // Set log file name to the service name
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("report_server/log");
  log_config.BaseName(service.Name().empty() ? "serviced" : service.Name());
  log_config.CreateDefaultLogger();
  LOG_DEBUG() << "Log File created. Path: " << log_config.GetLogFile();

  const auto read = service.ReadRegistryInfo();
  if (!read) {
    LOG_ERROR() << "Failed to read service daemon information from registry. Service: " << service.Name();
    return 0;
  }

  const auto run = service.RunService();
  if (!run) {
    LOG_ERROR() << "Failed to run service daemon . Service: " << service.Name();
    return 0;
  }

  return run ? 1 : 0;
}
