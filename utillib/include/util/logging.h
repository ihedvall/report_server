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

/** \brief Search and returns the path to the Notepad application..
 * Search primary for the Notepad++ application and second for the Notepad application.
 * @return The path to the Notepad++ or the Notepad application.
 */
std::string FindNotepad();

/** \brief Backup up a file with the 9 last changes.
 *
 * Backup a file by adding a sequence number 0..9 to the file (<file>_N.<ext>).
 * @param filename Full path to the file.
 * @param remove_file If set to true the file will be renamed to file_0. If set to
 * false the file will copy its content to file_0. The latter is slower but safer.
 * @return True if successful
 */
bool BackupFiles(const std::string &filename, bool remove_file = true);

}


