/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
/** \file ilogger.h
 * \brief Defines an interface against a generic logger.
 */
#pragma once
#include <string>
#include "logmessage.h"

namespace util::log {

/** \class ILogger ilogger.h "util/ilogger.h"
 * \brief Interface against a generic logger.
 *
 * The class is an interface class for implementing a logger. Its main interface is the AddLogMessage()
 * function that handles incoming log messages.
 */
class ILogger {
 public:
  virtual ~ILogger() = default; ///< Destructor
  virtual void AddLogMessage(const LogMessage &message) = 0; ///< Handle a log message
  virtual void Stop() = 0; ///< Stops any worker thread
  [[nodiscard]] virtual bool HasLogFile() const = 0; ///< Returns true if the logger has  file.
  [[nodiscard]] virtual std::string Filename() const = 0; ///< Return full path to the log file.
 protected:
  ILogger() = default; ///< Constructor
 private:

};

}
