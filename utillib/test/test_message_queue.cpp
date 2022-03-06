/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string_view>
#include <atomic>
#include <thread>
#include <array>
#include <chrono>
#include <gtest/gtest.h>
#include "messagequeue.h"
using namespace boost::interprocess;
using namespace std::chrono_literals;
using namespace util::log::detail;

namespace {

constexpr std::string_view kQueueName = "TestQueueXXX";
std::atomic<size_t> kTaskCount = 0;
std::atomic<bool> kStopTask = false;

void SendTask() {
  MessageQueue queue(false, kQueueName.data());
  for (size_t index = 0; index < 10; ++index) {
      SharedListenMessage msg;
      msg.ns1970 = index;
      strcpy(msg.text, "Text");
    queue.Add(msg);
    }
}

void ReceiveTask(MessageQueue* queue) {

  kTaskCount = 0;
  do {
    SharedListenMessage msg;
    const auto rec = queue->Get(msg, true);
    if (rec) {
      if (strcmp(msg.text, "Text") != 0) {
        std::cout << "Receiver Text Error: " << msg.text << std::endl;
        break;
      }
      if (msg.ns1970 > 100) {
        std::cout << "Receiver time Error: " << msg.ns1970 << std::endl;
        break;
      }
      ++kTaskCount;
    } else {
      break;
    }
  } while (!kStopTask);
}

} // end namespace

namespace util::test {

TEST(MessageQueue, TestBasic) {
  MessageQueue master(true, kQueueName.data());

  auto send_task = std::thread(&SendTask);
  send_task.join();
  EXPECT_EQ(master.NofMessages(), 10);
}

TEST(MessageQueue, TestMultipleSender) {

  MessageQueue master(true, kQueueName.data());
  kStopTask = false;
  auto server_task = std::thread(&ReceiveTask, &master);

  std::array<std::thread, 10> task_list;
  for (auto& task1 : task_list) {
    task1 = std::thread(&SendTask);
  }
  for (auto& task2 : task_list) {
    task2.join();
  }
  for (size_t count = 0; count < 1000 && kTaskCount < task_list.size() * 10; ++count) {
    std::this_thread::sleep_for(10ms);
  }

  kStopTask = true;
  master.Stop();
  server_task.join();

  std::cout << "Message Count: " << kTaskCount << std::endl;
  EXPECT_EQ(kTaskCount,task_list.size() * 10);
}
} // end namespace util::test
