/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>

#include <boost/endian/conversion.hpp>
#include <boost/endian/buffers.hpp>
#include "util/timestamp.h"

namespace util::time {

std::string GetLocalDateTime(std::chrono::time_point<std::chrono::system_clock> timestamp) {
  const auto utc = std::chrono::system_clock::to_time_t(timestamp);
  struct tm bt{};
  localtime_s(&bt, &utc);
  std::ostringstream date_time;
  date_time << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
  return date_time.str();
}

std::string GetLocalTimestampWithMs(std::chrono::time_point<std::chrono::system_clock> timestamp) {
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()) % 1000;
  const auto timer = std::chrono::system_clock::to_time_t(timestamp);
  struct tm bt{};
  localtime_s(&bt, &timer);

  std::ostringstream text;
  text << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
  return text.str();
}

std::string GetLocalTimestampWithUs(std::chrono::time_point<std::chrono::system_clock> timestamp) {
  const auto us = std::chrono::duration_cast<std::chrono::microseconds>(timestamp.time_since_epoch()) % 1000'000;
  const auto timer = std::chrono::system_clock::to_time_t(timestamp);
  struct tm bt{};
  localtime_s(&bt, &timer);

  std::ostringstream text;
  text << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(6) << us.count();
  return text.str();
}

uint64_t TimeStampToNs(TimeStamp timestamp) {
  // 1.na since midnight
  // 2 sec since 1970
  // 3 Add them
  const auto ns_midnight = std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch()) % 1000'000'000;
  const auto sec_1970 = std::chrono::system_clock::to_time_t(timestamp);
  uint64_t ns = sec_1970;
  ns *= 1'000'000'000;
  ns += ns_midnight.count();
  return ns;
}

std::string NanoSecToDDMMYYYY(uint64_t ns_since_1970) {
  auto system_time = static_cast<time_t>(ns_since_1970 / 1'000'000'000ULL);
  struct tm bt{};
  localtime_s(&bt, &system_time);
  std::ostringstream s;
  s << std::put_time(&bt, "%d:%m:%Y");
  return s.str();
}

std::string NanoSecToHHMMSS(uint64_t ns_since_1970) {
  const auto system_time = static_cast<time_t>(ns_since_1970 / 1'000'000'000ULL);
  struct tm bt{};
  localtime_s(&bt, &system_time);
  std::ostringstream s;
  s << std::put_time(&bt, "%H:%M:%S");
  return s.str();
}

uint64_t NanoSecToLocal(uint64_t ns_since_1970) {
  const auto utc_offset = TimeZoneOffset();
  return ns_since_1970 + (utc_offset * 1'000'000'000);
}

int64_t TimeZoneOffset() {
  const auto system_time = std::time(nullptr);
  struct tm utc{};
  gmtime_s( &utc, &system_time);
  utc.tm_isdst = -1;
  const auto local_time = mktime(&utc);
  return system_time - local_time;
}

std::vector<uint8_t> NsToCanOpenDateArray(uint64_t ns_since_1970)  {
  std::vector<uint8_t> date_array(7,0);
  const auto system_time = static_cast<time_t>(ns_since_1970 / 1'000'000'000); // Convert to sec since 1970
  const auto ms_min = static_cast<uint16_t>((ns_since_1970 / 1'000'000) % 60'000); // ms since minute shift

  boost::endian::little_uint16_buf_at data(ms_min);
  memcpy(date_array.data(), data.data(), 2);

  struct tm bt{};
  localtime_s(&bt, &system_time);
  date_array[2] = bt.tm_min;
  date_array[3] = bt.tm_hour;
  if (bt.tm_isdst) {
    date_array[3] |= 0x80;
  }
  date_array[4] = bt.tm_mday;
  uint8_t day_in_week = bt.tm_wday == 0 ? 7 : bt.tm_wday;
  date_array[4] |= (day_in_week << 5);
  date_array[5] = 1 + bt.tm_mon;
  date_array[6] = bt.tm_year % 100;
  return std::move(date_array);
}

uint64_t CanOpenDateArrayToNs(const std::vector<uint8_t> &buffer) {
  boost::endian::little_uint16_buf_t ms_min; // ms since last minute
  memcpy(ms_min.data(), buffer.data(), 2); // Note ms since last minute not seconds as normal
  const uint8_t min = buffer[2] & 0x3F;
  const uint8_t hour = buffer[3] & 0x1F;
  const bool dst = (0x80 & buffer[3]) != 0;
  const uint8_t day_in_week = (buffer[4] & 0xE0) >> 5; // 1 Monday..7 = Sunday
  const uint8_t day = buffer[4] & 0x1F; // 1..31

  const uint8_t month = buffer[5] & 0x3F; // 1-12
  const uint8_t year = buffer[6] & 0x7F; // 0.99

  // Convert to milliseconds since 1970

  uint64_t dest = static_cast<uint64_t>(ms_min.value()) * 1'000'000;

  struct tm time {};
  time.tm_sec = 0;
  time.tm_min = min;
  time.tm_hour = hour;
  time.tm_mday = day; // 1..31
  time.tm_mon = month - 1; // Note 0..11
  time.tm_year = year >= 70 ? year : 100 + year; // Years since 1900
  time.tm_wday = day_in_week == 7 ? 0 : day_in_week; // 0 = Sunday..6 = Saturday. Actually ignored by mktime()
  time.tm_yday = 0; // Ignored by mktime()
  time.tm_isdst = dst ? 1 : 0;

  std::time_t sec_1970 = std::mktime(&time); // Hope that epoch is 1970
  dest += static_cast<uint64_t>(sec_1970) * 1'000'000'000;
  return dest;
}

std::vector<uint8_t> NsToCanOpenTimeArray(uint64_t ns_since_1970)  {
  // Time Array Format (6 byte, little endian):
  // uint32_t ms since midnight
  // uint16_t days since 1984-01-01
  std::vector<uint8_t> time_array(6,0);

  // Calculate second to 1984-01-01
  struct tm bt{};
  bt.tm_year = 84;
  bt.tm_mon = 0;
  bt.tm_mday = 1;
  const auto sec_1984 = mktime(&bt);

  uint64_t ms_since_1984 = ns_since_1970 / 1'000'000;
  ms_since_1984 -= static_cast<uint64_t>(sec_1984) * 1'000;

  const auto days_since_1984 = static_cast<uint16_t>(ms_since_1984 / (24 * 3'600'000));

  boost::endian::little_uint32_buf_t milli_sec(static_cast<uint32_t>(ms_since_1984 % (24 * 3'600'000)));
  memcpy(time_array.data(), milli_sec.data(), 4);

  boost::endian::little_uint16_buf_t days(days_since_1984);
  memcpy(time_array.data() + 4, days.data(), 2);

  return time_array;
}

uint64_t CanOpenTimeArrayToNs(const std::vector<uint8_t> &buffer) {
  boost::endian::little_uint32_buf_t ms; // Milliseconds since midnight
  memcpy(ms.data(), buffer.data(), 4);
  const auto ms1 = ms.value();

  boost::endian::little_uint16_buf_t days; // Days since 1984-01-01
  memcpy(days.data(), buffer.data() + 4, 2);

  struct tm time {};
  time.tm_mday = 1; // 1..31
  time.tm_mon = 0; // Note 0..11
  time.tm_year = 84; // Years since 1900

  const auto sec_1984 = std::mktime(&time); // Hope that epoch is 1970
  uint64_t dest = static_cast<uint64_t>(sec_1984) * 1'000'000'000;
  dest += static_cast<uint64_t>(days.value()) * 24 * 3'600'000'000'000;
  dest += static_cast<uint64_t>(ms.value()) * 1'000'000;
  return dest;
}

std::string NsToLocalIsoTime(uint64_t ns_since_1970) {
  const auto ms_sec = (ns_since_1970 / 1'000'000) % 1'000;
  const auto system_time = static_cast<std::time_t>(ns_since_1970 / 1'000'000'000);
  struct tm bt{};
  localtime_s(&bt, &system_time);
  std::ostringstream text;
  text << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms_sec;
  return text.str();
}

std::string NsToLocalDate(uint64_t ns_since_1970) {
  const auto system_time = static_cast<std::time_t>(ns_since_1970 / 1'000'000'000);
  struct tm bt{};
  localtime_s(&bt, &system_time);
  std::ostringstream text;
  text << std::put_time(&bt, "%x");
  return text.str();
}

std::string NsToLocalTime(uint64_t ns_since_1970, int format) {
  const auto system_time = static_cast<std::time_t>(ns_since_1970 / 1'000'000'000);
  struct tm bt{};
  localtime_s(&bt, &system_time);
  std::ostringstream text;
  text << std::put_time(&bt, "%X");

  std::ostringstream extra;
  switch (format) {
    case 1: { // Show ms
      const auto ms_sec = (ns_since_1970 / 1'000'000) % 1'000;
      extra << '.' << std::setfill('0') << std::setw(3) << ms_sec;
      break;
    }

    case 2: { // Show us
      const auto us_sec = (ns_since_1970 / 1'000) % 1'000'000;
      extra << '.' << std::setfill('0') << std::setw(6) << us_sec;
      break;
    }

    case 3: { // Show ns
      const auto ns_sec = (ns_since_1970) % 1'000'000'000;
      extra << '.' << std::setfill('0') << std::setw(9) << ns_sec;
      break;
    }

    case 0: // Show seconds
    default:
      return text.str();
  }

  const std::string input = text.str();
  std::ostringstream output;
  bool is_am_pm = false;
  bool is_blank = false;
  for (const char char_in : input) {
    if (std::isalpha(char_in) && !is_am_pm) { // Assume AM PM and insert ms
      output << extra.str();
      if (is_blank) {
        output << " ";
        is_blank = false;
      }
      is_am_pm = true;
      output << char_in;
    } else if (std::iswspace(char_in)) {
      is_blank = true;
    } else {
      if (is_blank) {
        output << " ";
        is_blank = false;
      }
      output << char_in;
    }
  }
  if (!is_am_pm) {
    output << extra.str();
  }
  return output.str();
}

}
