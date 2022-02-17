/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <chrono>
#include <string_view>
#include "util/ilisten.h"
#include "util/timestamp.h"
#include "util/logconfig.h"
#include "testlisten.h"
#include "listenserver.h"
#include "listenconfig.h"

using namespace util::log;
using namespace util::log::detail;
using namespace std::chrono_literals;

namespace {
  constexpr uint64_t kServerPort = 64999;
  constexpr std::string_view kServerName = "TestServer";
  constexpr std::string_view kServerPreText = "TS>";
}
namespace util::test {
void TestListen::SetUpTestCase() {
  auto &log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();
}

void TestListen::TearDownTestCase()
{
  auto &log_config = LogConfig::Instance();
  log_config.DeleteLogChain();
}

bool ListenMock::IsActive() const {
  return true;
}

void ListenMock::AddMessage(uint64_t nano_sec_1970, const std::string &pre_text, const std::string &text) {
  std::cout << time::NsToLocalIsoTime(nano_sec_1970) << " " << pre_text << " " << text << std::endl;
}

size_t ListenMock::LogLevel() {
  return 0;
}

TEST_F(TestListen, ListenBasic) {
  ListenMock listen;
  listen.PreText("TEST>");


  listen.ListenText("Test text %d", 12);
  listen.ListenTextEx(0, "NULL>", "Test text %d", 33);
  const std::vector<uint8_t> buffer = {0,1,2,3,4,5,6,7};
  listen.ListenTransmit(time::TimeStampToNs(), "T>", buffer, nullptr);
  listen.ListenReceive(time::TimeStampToNs(), "R>", buffer, nullptr);
}

TEST_F(TestListen, ListenServer) {
  ListenServer server;
  server.Name(kServerName.data());
  server.Description("Test server");
  server.HostName("127.0.0.1");
  server.Port(kServerPort);

  const bool start = server.Start();
  EXPECT_TRUE(start);

  std::this_thread::sleep_for(60s);

  const bool stop = server.Stop();
  EXPECT_TRUE(stop);
}

TEST_F(TestListen, ListenConfig) {
  ListenPortConfig devils_port;
  devils_port.port = 666;
  devils_port.name = "Devils Gate";
  devils_port.share_name = "Trump";
  devils_port.description = "Republican";

  {
    ListenConfig master;

    auto list_empty = GetListenConfigList();
    EXPECT_TRUE(list_empty.empty());

    AddListenConfig(devils_port);
    auto list_one = GetListenConfigList();
    EXPECT_EQ(list_one.size(), 1);

    DeleteListenConfig(devils_port.port);
    auto list_none = GetListenConfigList();
    EXPECT_TRUE(list_none.empty());
  }
  {
    AddListenConfig(devils_port);
    DeleteListenConfig(devils_port.port);
  }
}

} // end namespace