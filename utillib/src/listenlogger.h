/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "util/ilogger.h"
#include "util/ilisten.h"
#include "listenproxy.h"

namespace util::log::detail {

class ListenLogger : public ILogger {
 public:
  ListenLogger();
  void AddLogMessage(const LogMessage &message) override;
  [[nodiscard]] bool HasLogFile() const override; ///< Returns true if it have a log file.
  [[nodiscard]] std::string Filename() const override; ///< Returns the log file
  void Stop() override; ///< Stops any working thread.
 private:
  ListenProxy listen_proxy_;
};

}



