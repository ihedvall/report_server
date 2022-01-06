/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <chrono>
#include <thread>
#include "messagequeue.h"

using namespace boost::interprocess;
using namespace std::chrono_literals;

namespace util::log::detail {

MessageQueue::MessageQueue(bool master, const std::string &shared_mem_name)
  : master_(master),
    name_(shared_mem_name) {
  try {
    if (master_) {
      shared_memory_object::remove(shared_mem_name.c_str());
      shared_mem_ = std::make_unique<shared_memory_object>(create_only, shared_mem_name.c_str(), read_write);
      shared_mem_->truncate(sizeof(SharedListenQueue));
      region_ = std::make_unique<mapped_region>(*shared_mem_, read_write);
      queue_ = new (region_->get_address()) SharedListenQueue;
      queue_->active = false;
    } else {
      active_ = false;
      task_ = std::thread(&MessageQueue::ClientTask,this);
    }

  } catch (const std::exception& error) {
    queue_ = nullptr;
    region_.reset();
    shared_mem_.reset();
  }
}


MessageQueue::~MessageQueue() {
  task_stop_ = true;
  if (queue_ != nullptr) {
    // Just in case any client is holding the area, the remove will fail
    queue_->active = false;
    queue_->log_level = 0;
    queue_->message_semaphore.post(); // Generate a fake message to wake-up GetMessage() waits
  }
  if (task_.joinable()) {
    task_event_.notify_one();
    task_.join();
  }
  queue_ = nullptr;
  region_.reset();
  shared_mem_.reset();

  if (master_) {
    shared_memory_object::remove(name_.c_str());
  }
}

void MessageQueue::Add(const SharedListenMessage &msg) {
  try {
    bool add = false;
    do {
      shared_memory_object shared_mem(open_only, name_.c_str(), read_write);
      mapped_region region(shared_mem, read_write);
      auto* queue = static_cast<SharedListenQueue *> (region.get_address());
      if (queue == nullptr) {
        return;
      }
      auto &mem = *queue;
      if (task_stop_) {
        return;
      }
      scoped_lock lock(mem.locker);

      if (mem.nof_messages > 255) {
        lock.unlock();
        std::this_thread::sleep_for(1ms);
        lock.lock();
      } else {
        auto &in_msg = mem.queue[mem.queue_in];
        in_msg = msg;
        ++mem.queue_in;
        mem.message_semaphore.post();
        ++mem.nof_messages;
        add = true;
      }
    } while (!add);

  } catch (const std::exception &) {
  }
}

bool MessageQueue::Get(SharedListenMessage &msg, bool block) {

  if (task_stop_ || !master_ || queue_ == nullptr) {
    return false;
  }
  auto &mem = *queue_;
  if (block) {
    mem.message_semaphore.wait();
    if (!task_stop_ && mem.nof_messages > 0) {
      scoped_lock lock(mem.locker);
      const auto &out_msg = mem.queue[mem.queue_out];
      msg = out_msg;
      ++mem.queue_out;
     --mem.nof_messages;
      return true;
   }
  } else {
    const bool message = mem.message_semaphore.try_wait();
    if (message && !task_stop_ && mem.nof_messages > 0) {
      scoped_lock lock(mem.locker);
      const auto &out_msg = mem.queue[mem.queue_out];
      msg = out_msg;
      ++mem.queue_out;
      --mem.nof_messages;
      return true;
    }
  }
  return false;
}

size_t MessageQueue::NofMessages() const {
  try {
    shared_memory_object shared_mem(open_only, name_.c_str(), read_write);
    mapped_region region(shared_mem, read_write);
    auto *queue = static_cast<SharedListenQueue *> (region.get_address());
    if (queue != nullptr) {
      return queue->nof_messages;
    }
  } catch (const std::exception& ) {
  }
  return 0;
}

void MessageQueue::ClientTask() {
  while (!task_stop_) {
    try {
      shared_memory_object shared_mem(open_only, name_.c_str(), read_write);
      mapped_region region(shared_mem, read_write);
      const auto* queue = static_cast<SharedListenQueue *> (region.get_address());
      if (queue == nullptr) {
        active_ = false;
        log_level_ = 0;
      } else {
        active_ = queue->active.load();
        log_level_ = queue->log_level.load();
      }
    } catch (const std::exception&) {
      active_ = false;
      log_level_ = 0;
    }
    std::unique_lock<std::mutex> lock(task_mutex_);
    task_event_.wait_for(lock, 1s, [&] { return task_stop_.load(); });
   }
}

void MessageQueue::Stop() {
  task_stop_ = true;
  task_event_.notify_one();
  if (task_.joinable()) {
    task_.join();
  } else {

    try {
      shared_memory_object shared_mem(open_only, name_.c_str(), read_write);
      mapped_region region(shared_mem, read_write);
      auto *queue = static_cast<SharedListenQueue *> (region.get_address());
      if (queue != nullptr) {
        queue->active = false;
        queue->message_semaphore.post();
      }
    } catch (const std::exception &) {
    }
  }

}

void MessageQueue::SetActive(bool active) {
  active_ = active;
  if (queue_ != nullptr) {
    queue_->active = active;
  }
}

void MessageQueue::SetLogLevel(uint8_t log_level) {
  log_level_ = log_level;
  if (queue_ != nullptr) {
    queue_->log_level = log_level;
  }
}

} // end namespace util::detail
