/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/** \file timestamp.h
 * \brief Contains various conversion between different time formats.
 *
 * The central time is an uint64_t UTC nanoseconds since 1970-01-01. This is an extension of the ols time_t
 * that is seconds since 1970-01-01. In general the std::chrono::system_clock is used as reference.
 *
 * The nanosecond timestamp is used in communications and a general timestamp. This due to the fact that
 * different system used different epoch for system clock. Note most system uses 1970-01-01 as epoch and
 * is called POSIX time or UNIX time.
 *
 * \note
 * The std::chrono library have many new functions in C++20. Unfortunately not many compiler have implemented
 * these function so most most of the implementation uses C++17 functionality.
 */
#pragma once
#include <string>
#include <chrono>
#include <vector>


namespace util::time {
/** \typedef SystemClock
 * \brief Convenience type definition for std::chrono::system_clock;
 */
 using SystemClock = std::chrono::system_clock;

/** \typedef TimeStamp
 * \brief Convenience type definition for std::chrono::time_point<std::chrono::system_time>;
 */
 using TimeStamp = std::chrono::time_point<SystemClock>;

 /** \brief returns a ISO date and time string.
  *
  * The function is intended to be used for time stamp in log files.
  *
  * @param [in] timestamp System clock timestamp
  * @return Returns a date and time string.
  */
std::string GetLocalDateTime(TimeStamp timestamp = SystemClock::now());

/** \brief returns a ISO date and time string.
 *
 * The function is intended to be used for time stamp in log files.
 *
 * @param [in] timestamp System clock timestamp
 * @return Returns a date and time string.
 */
std::string GetLocalTimestampWithMs(TimeStamp timestamp = SystemClock::now());

/** \brief returns a ISO date and time string.
 *
 * The function is intended to be used for time stamp in log files.
 *
 * @param [in] timestamp System clock timestamp
 * @return Returns a date and time string.
 */
std::string GetLocalTimestampWithUs(TimeStamp timestamp = SystemClock::now());

/** \brief Converts a nanosecond since 1970 to a local ISO date and time string.
 *
 * @param [in] ns_since_1970 Nanosecond since 1970
 * @return Return a date and time string in format YYYY-MM-DD hh:mm:ss.ms
 */
std::string NsToLocalIsoTime(uint64_t ns_since_1970);

/** \brief Converts a nanosecond since 1970 to a UTC ISO date and time string.
 *
 * @param [in] ns_since_1970 Nanosecond since 1970
 * @return Return a date and time string in format YYYY-MM-DDThh:mm:ss.sssssssssZ
 */
std::string NsToIsoTime(uint64_t ns_since_1970, int format = 0);

/** \brief Converts an ISO UTC string to nanosecond since 1970.
 *
 * @param [in] ISO Time stamp string YYYY-MM-DDThh:mm:ss.sssssssssZ
 * @param [in] format 0: Include seconds, 1: Include ms, 2: include micro-seconds, 3: Include ns
 * @return Return a date and time string in format
 */
uint64_t IsoTimeToNs(const std::string& iso_time);

/** \brief Converts system clock to ns since midnight 1970.
 *
 *
 * @param timestamp System clock time stamp
 * @return Nanoseconds since midnight 1970
 */
uint64_t TimeStampToNs(TimeStamp timestamp = SystemClock::now());

/** \brief Converts ns since 1970 UTC to local date DD:MM:YYYY string
 *
 * Generates a local date string with the 'DD:MM:YYYY' format, from an UTC time stamp nanoseconds since
 * 1970-01-01 (midnight). The nanosecond  timestamp is commonly used when transfer a timestamp. Note that this may
 * not be used as epoch in different internal clocks.
 *
 * @param [in] ns_since_1970 Nanoseconds since 1970 UTC
 * @return Local date format 'DD:MM:YYYY'
 */
std::string NanoSecToDDMMYYYY(uint64_t ns_since_1970);

/** \brief Converts ns since 1970 UTC to local time HH:MM:SS string
 *
 * Generates a local time string with the 'HH:MM:ss' format, from an UTC time stamp nanoseconds since
 * 1970-01-01 (midnight). The nanosecond  timestamp is commonly used when transfer a timestamp. Note that this may
 * not be used as epoch in different internal clocks.
 *
 * @param [in] ns_since_1970 Nanoseconds since 1970 UTC
 * @return Local date format 'HH:MM:SS'
 */
std::string NanoSecToHHMMSS(uint64_t ns_since_1970);

/** \brief Adds the time zone offset to the time stamp.
 *
 * Adds the time zone offset to the UTC nanoseconds since 1970.
 * @param [in] ns_since_1970
 * @return local time = system time + time zone offset
 */
uint64_t NanoSecToLocal(uint64_t ns_since_1970);

/** \brief Converts from nanoseconds to CANopen 7 byte Date array.
 *
 * Converts from nanoseconds since 1970-01-01 to a 7 byte CANopen date array
 * This format is used in CANopen protocol and in ASAM MDF files.
 *
 * 7-byte CANopen date format:
 * \li uint16_t Milliseconds since last minute
 * \li uint8_t Minute (0..59)
 * \li uint8_t Hour (0..23)
 * \li uint8_t Day in month (1..31) + Day in week (1 = Monday..7 = Sunday)
 * \li uint8_t Month (1..12)
 * \li uint8_t Year (0..99)
 * @param [in] ns_since_1970 Nanoseconds since 1970-01-01
 * @return 7-byte CANopen date array
 */
 std::vector<uint8_t> NsToCanOpenDateArray(uint64_t ns_since_1970);

/** \brief Converts from a CANopen 7 byte Date array to nanoseconds since 1970.
 *
 * Converts from 7 byte CANopen date array to a uint64_t nanoseconds since 1970.
 * This format is used in CANopen protocol and in ASAM MDF files.
 *
 * 7-byte CANopen date format:
 * \li uint16_t Milliseconds since last minute
 * \li uint8_t Minute (0..59)
 * \li uint8_t Hour (0..23)
 * \li uint8_t Day in month (1..31) + Day in week (1 = Monday..7 = Sunday)
 * \li uint8_t Month (1..12)
 * \li uint8_t Year (0..99)
 * @param [in] buffer 7-byte CANopen date array
 * @return Nanoseconds since 1970-01-01
 */
uint64_t CanOpenDateArrayToNs(const std::vector<uint8_t> &buffer);

/** \brief Converts from nanoseconds to CANopen 6 byte Time array.
 *
 * Converts from nanoseconds since 1970-01-01 to a 6-byte CANopen time array
 * This format is used in CANopen protocol and in ASAM MDF files.
 *
 * 6-byte CANopen time format:
 * \li uint32_t Milliseconds since midnight
 * \li uint16_t Days since 1984

 * @param [in] ns_since_1970 Nanoseconds since 1970-01-01
 * @return 6-byte CANopen date array
 */
std::vector<uint8_t> NsToCanOpenTimeArray(uint64_t ns_since_1970);

/** \brief Formats a date string according to the local date format.
 *
 * Formats a date string according to the local date format.
 * @param ns_since_1970 Number of nanoseconds since 1970
 * @return Localized date string
 */
std::string NsToLocalDate(uint64_t ns_since_1970);

/** \brief Formats a time string according to the local time format.
 *
 * Formats a time string with optional ms..ns. The output format is according to the
 * local time format.
 * @param ns_since_1970 Number of nanoseconds since 1970
 * @param format 0 = Show only seconds, 1 = Show ms, 2 = Show microseconds, 3 = Show ns
 * @return Localized time string
 */
std::string NsToLocalTime(uint64_t ns_since_1970, int format = 0);

/** \brief Converts from a CANopen 6 byte Time array to nanoseconds since 1970.
 *
 * Converts from nanoseconds since 1970-01-01 to a 6-byte CANopen time array
 * This format is used in CANopen protocol and in ASAM MDF files.
 *
 * 6-byte CANopen time format:
 * \li uint32_t Milliseconds since midnight
 * \li uint16_t Days since 1984

 * @param [in] buffer  6-byte CANopen time array
 * @return Nanoseconds since 1970-01-01
 */
uint64_t CanOpenTimeArrayToNs(const std::vector<uint8_t> &buffer);

/** \brief return the time zone offset in seconds.
 *
 * Returns the current used time zone offset in seconds
 * @return
 */
int64_t TimeZoneOffset();

/** \brief Converts an ODS date and time string to nanoseconds since 1970.
 *
 * Converts an ODS date and time string to nanoseconds since 1970. The ODS
 * DT_DATE string have the format 'YYYYMMDDhhmmsslllcccnnn'.
 * @param ods_date ODS date string
 * @return Nanoseconds since 1970
 */
uint64_t OdsDateToNs(const std::string& ods_date);

}
