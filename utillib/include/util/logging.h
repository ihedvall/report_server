/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file logging.h
 * \brief Standard log interfaces.
 */
#pragma once
#include <cstdarg>
#include <cstdio>
#include <string>

#if __has_builtin(__builtin_source_location) //__has_include("source_location")
#include <source_location>
#else
#include <experimental/source_location>
#endif

namespace util::log {
///< Defines the log severity level
enum class LogSeverity {
  kTrace = 0, ///< Trace or listen message
  kDebug,     ///< Debug message
  kInfo,      ///< Informational message
  kWarning,   ///< Warning message
  kError,     ///< Error message
  kCritical,  ///< Critical message (device error)
  kAlert,     ///< Alert or alarm message
  kEmergency  ///< Fatal error message
};
/** \typedef Loc
 * The Loc is a wrapper around the std::location library. This library is new in C++20 and some
 * treat is as experimental.
 */
#if __has_builtin(__builtin_source_location) //__has_include("source_location")

typedef std::source_location Loc;
#else
typedef std::experimental::source_location Loc;
#endif

void LogDebug(const Loc &loc, const char *fmt, ...); ///< Creates a debug message message
void LogInfo(const Loc &loc, const char *fmt, ...); ///< Creates an information message
void LogError(const Loc &loc, const char *fmt, ...); ///< Creates an error message
void LogString(const Loc &loc, LogSeverity severity,
               const std::string &message); ///< Creates a generic message

std::string FindNotepad();
}


