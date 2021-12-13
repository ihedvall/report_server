/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include "util/logmessage.h"

namespace util::log {
std::string GetSeverityString(LogSeverity severity) {
  switch (severity) {
    case LogSeverity::kTrace:return "[Trace]";

    case LogSeverity::kDebug:return "[Debug]";

    case LogSeverity::kInfo:return "[Info]";

    case LogSeverity::kWarning:return "[Warning]";

    case LogSeverity::kError:return "[Error]";

    case LogSeverity::kFatal:return "[Fatal]";

    default:break;
  }
  return "[Unknown]";
}
}