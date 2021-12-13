/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <atomic>
#include <thread>
#include <gtest/gtest.h>
#include "util/logging.h"
#include "util/logstream.h"
#include "util/logconfig.h"

using namespace util::log;

namespace {

std::atomic<int> jj = 0;

void TestLogInfo(int ii) {
  LOG_INFO() << "Olle: " << ii;
  LOG_ERROR() << "Olle: " << ii;
  LogInfo(Loc::current(), "Pelle %d", ii);
  LogError(Loc::current(), "Pelle: %d", ii);
}

void TestThreadFunction() {
  while (jj < 1000) {
    int ii = jj++;
    LOG_INFO() << "Nisse: " << ii;
  }
}

}

namespace util::test {
TEST(Logging, Console) //NOLINT
{
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();
  for (int ii = 0; ii < 10; ++ii)
    TestLogInfo(ii);
  log_config.DeleteLogChain();
}

TEST(Logging, ConsoleMutiThread) //NOLINT
{
  jj = 0;
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();
  std::array<std::thread, 100> thread_list;
  for (auto &t: thread_list) {
    t = std::thread(TestThreadFunction);
  }
  for (auto &t: thread_list) {
    t.join();
  }
  log_config.DeleteLogChain();
}

TEST(Logging, ConsolePerformance) //NOLINT
{
  jj = 0;
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  TestThreadFunction(); // Reuse of the threading function

  log_config.DeleteLogChain();
}

TEST(Logging, LogToFile) //NOLINT
{
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.BaseName("test.log");
  log_config.SubDir("Testing");
  log_config.CreateDefaultLogger();
  for (int ii = 0; ii < 10; ++ii)
    TestLogInfo(ii);
  log_config.DeleteLogChain();
}

TEST(Logging, LogToFileMutiThread) //NOLINT
{
  jj = 0;
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.BaseName("test.log");
  log_config.SubDir("Testing");
  log_config.CreateDefaultLogger();
  std::array<std::thread, 100> thread_list;
  for (auto &t: thread_list) {
    t = std::thread(TestThreadFunction);
  }
  for (auto &t: thread_list) {
    t.join();
  }
  log_config.DeleteLogChain();
}

TEST(Logging, DISABLED_LogToFilePerformance) //NOLINT
{
  jj = 0;
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.BaseName("test.log");
  log_config.SubDir("Testing");
  log_config.CreateDefaultLogger();

  TestThreadFunction(); // Reuse of the threading function

  log_config.DeleteLogChain();
}

TEST(Logging, DISABLED_LogToFileBackup) //NOLINT
{
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.BaseName("test.log");
  log_config.SubDir("Testing");
  log_config.CreateDefaultLogger();
  for (int ii = 0; ii < 1000'000; ++ii)
    TestLogInfo(ii);

  log_config.DeleteLogChain();
}
}