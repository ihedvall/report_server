/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include <string>
#include <chrono>
#include <limits>
#include <ctime>
#include <gtest/gtest.h>
#include "util/timestamp.h"
using namespace util::time;
using namespace std::chrono;
using namespace std::chrono_literals;
namespace {

const std::string kDateTime = "YYYY-MM-DD hh:mm:ss"; //NOLINT
const std::string kTimestampMs("YYYY-MM-DD hh:mm:ss.xxx"); //NOLINT
const std::string kTimestampUs("YYYY-MM-DD hh:mm:ss.xxxyyy"); // NOLINT
const std::string kMdf3Date("DD:MM:YYYY"); // NOLINT
const std::string kMdf3Time("HH:MM:SS"); // NOLINT

}
namespace util::test  {

TEST(Timestamp, TestEpoch) { // NOLINT
  {
    time_t time_t_epoch = 0;
    struct tm bt{};
    gmtime_s(&bt, &time_t_epoch);
    std::ostringstream date_time;
    date_time << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
    std::cout << "Time_t Epoch: " << date_time.str() << std::endl;
  }

  {
    const auto timestamp_1970 = SystemClock::from_time_t(0);
    const auto offset_epoch = timestamp_1970.time_since_epoch();
    std::cout << "System Clock Epoch [ns]: " << offset_epoch.count() << std::endl;
  }

  {
    const time_t max_time = std::numeric_limits<time_t>::max() / 300'000'000;
    struct tm bt{};
    gmtime_s(&bt, &max_time);
    std::ostringstream date_time;
    date_time << std::put_time(&bt, "%Y-%m-%d %H:%M:%S") ;
    std::cout << "Max Time_t: " << date_time.str() << std::endl;
  }

  {
    const uint64_t max = std::numeric_limits<uint64_t>::max();
    const auto max_time = static_cast<time_t>(max / 1'000'000'000);
    struct tm bt{};
    gmtime_s(&bt, &max_time);
    std::ostringstream date_time;
    date_time << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
    std::cout << "Max (ns) Time: " << date_time.str() << std::endl;
  }
}

TEST(Timestamp, GetCurrentDateTime) //NOLINT
{
  const auto date_time = GetLocalDateTime();
  EXPECT_EQ(date_time.size(), kDateTime.size()) << date_time;
}

TEST(Timestamp, GetCurrentTimestampWithMs) //NOLINT
{
  const auto timestamp = GetLocalTimestampWithMs();
  EXPECT_EQ(timestamp.size(), kTimestampMs.size()) << timestamp;
}

TEST(Timestamp, GetCurrentTimestampWithUs) // NOLINT
{
  const auto timestamp = GetLocalTimestampWithUs();
  EXPECT_EQ(timestamp.size(), kTimestampUs.size()) << timestamp;
}

TEST(Timestamp, TimeStampToNs) // NOLINT
{
  {
    const auto timestamp = SystemClock::now();
    const auto system_time = SystemClock::to_time_t(timestamp);
    auto ns_1970 = TimeStampToNs(timestamp);
    std::cout << "TimeStampToNs (now): " << ns_1970 << std::endl;
    EXPECT_EQ(system_time, static_cast<time_t>(ns_1970 / 1'000'000'000));
  }
  {
    const auto timestamp = SystemClock::from_time_t(0);
    const auto system_time = SystemClock::to_time_t(timestamp);
    auto ns_1970 = TimeStampToNs(timestamp);
    std::cout << "TimeStampToNs (epoch): " << ns_1970 << std::endl;
    EXPECT_EQ(system_time, static_cast<time_t>(ns_1970 / 1'000'000'000));
  }


}

TEST(Timestamp, Mdf3Date) // NOLINT
{
  const auto timestamp = SystemClock::now();
  const auto ns = TimeStampToNs(timestamp);
  const auto local_date = GetLocalDateTime(timestamp);
  const auto mdf_date = NanoSecToDDMMYYYY(ns);

  std::cout << "MDF Date: " << mdf_date << ", Local Date: " << local_date <<  std::endl;

  EXPECT_EQ(mdf_date.size(),kMdf3Date.size());
}

TEST(Timestamp, Mdf3Date_epoch) // NOLINT
{
  const auto mdf_date = NanoSecToDDMMYYYY(0);
  std::cout << "MDF Epoch: " << mdf_date << std::endl;

  EXPECT_EQ(mdf_date.size(),kMdf3Date.size());
}

TEST(Timestamp, Mdf3Time) // NOLINT
{
  const auto timestamp = SystemClock::now();
  const auto ns = TimeStampToNs(timestamp);
  const auto local_date = GetLocalDateTime(timestamp);
  const auto mdf_time = NanoSecToHHMMSS(ns);

  std::cout << "MDF Time: " << mdf_time << ", Local Date: " << local_date <<  std::endl;

  EXPECT_EQ(mdf_time.size(),kMdf3Time.size());
}

TEST(Timestamp, TimeZoneOffset) // NOLINT
{
  const auto offset = TimeZoneOffset();
  std::cout << "Time Zone Offset: " << offset <<  std::endl;
}

TEST(Timestamp, CANopenDate) { //NOLINT
  {
    const auto before = TimeStampToNs();
    const auto date_array = NsToCanOpenDateArray(before);
    const auto after = CanOpenDateArrayToNs(date_array);
    EXPECT_EQ(before / 1'000'000, after /1'000'000);
  }
  {
    const auto before = 0;
    const auto date_array = NsToCanOpenDateArray(before);
    const auto after = CanOpenDateArrayToNs(date_array);
    EXPECT_EQ(before / 1'000'000, after /1'000'000);
  }
}

TEST(Timestamp, CANopenTime) { //NOLINT
  {
    const auto before = TimeStampToNs();
    const auto time_array = NsToCanOpenTimeArray(before);
    const auto after = CanOpenTimeArrayToNs(time_array);
    EXPECT_EQ(before / 1'000'000, after /1'000'000);
  }
}

} // namespace util::test