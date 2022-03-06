/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <cstring>
#include <string_view>
#include <array>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include "util/logstream.h"
#include "util/listenconfig.h"

using namespace boost::interprocess;

namespace {
  constexpr std::string_view kShareMemName = "LISCONFIG";

  struct ShareListenPortConfig {
    bool used = false;
    uint16_t port = 0;
    char name[40] {};
    char share_name[40] {};
    char description[80] {};
  };

  struct ShareListenConfig {
    boost::interprocess::interprocess_recursive_mutex locker;
    uint16_t version = 0; // NOLINT
    std::array<ShareListenPortConfig, 256> config_list;
  };
}

namespace util::log {

ListenConfig::ListenConfig() {

  try {
    shared_memory_object::remove(kShareMemName.data());
    shared_memory_object shared_mem(create_only, kShareMemName.data(), read_write);
    shared_mem.truncate(sizeof(ShareListenConfig));
    mapped_region region(shared_mem, read_write);
    auto* config = new (region.get_address()) ShareListenConfig;
    scoped_lock lock(config->locker);
    config->version = 0;
    for (auto& cfg : config->config_list) {
      cfg.used = false;
      cfg.port = 0;
      memset(cfg.name,'\0',sizeof(cfg.name));
      memset(cfg.share_name,'\0',sizeof(cfg.share_name));
      memset(cfg.description,'\0',sizeof(cfg.description));
    }
  } catch (const std::exception& error) {
    LOG_ERROR() << "Failed to setup and initialize the shared memory. Error: " << error.what();
  }
}

ListenConfig::~ListenConfig() {
  try {
    shared_memory_object::remove(kShareMemName.data());
  } catch (const std::exception& error) {
    LOG_ERROR() << "Failed to remove the shared memory. Error: " << error.what();
  }
}

std::vector<ListenPortConfig> GetListenConfigList() {

  std::vector<ListenPortConfig> list;
  try {
    shared_memory_object shared_mem(open_only, kShareMemName.data(), read_write);
    mapped_region region(shared_mem, read_write);
    auto* config = static_cast<ShareListenConfig*> (region.get_address());
    scoped_lock lock(config->locker);
    for (auto& cfg : config->config_list) {
      if (cfg.used) {
        ListenPortConfig port_config;
        port_config.port = cfg.port;
        port_config.name = cfg.name;
        port_config.share_name = cfg.share_name;
        port_config.description = cfg.description;
        list.emplace_back(port_config);
      }
    }
  } catch (const std::exception& error) {
    LOG_INFO() << "Failed to connect to the shared memory. Error: " << error.what();
  }
  std::sort(list.begin(), list.end());
  return list;
}

void AddListenConfig(const ListenPortConfig &port_config) {
  if (port_config.port == 0) {
    return;
  }

  try {
    shared_memory_object shared_mem(open_only, kShareMemName.data(), read_write);
    mapped_region region(shared_mem, read_write);
    auto* config = static_cast<ShareListenConfig*> (region.get_address());
    scoped_lock lock(config->locker);

    // First check if port exist. If so update the configuration
    for (auto& cfg : config->config_list) {
      if (cfg.used && port_config.port == cfg.port) {
        if (port_config.name != cfg.name) {
          LOG_ERROR() << "Multiple listen IP ports registered with different names. Port: " << cfg.port
            << ", Names: " << cfg.name << "/" << port_config.name;
        }
        cfg.port = port_config.port;

        strncpy(cfg.name, port_config.name.c_str(), sizeof(cfg.name));
        cfg.name[sizeof(cfg.name) - 1] = '\0';

        strncpy(cfg.share_name, port_config.share_name.c_str(), sizeof(cfg.share_name));
        cfg.share_name[sizeof(cfg.share_name) - 1] = '\0';

        strncpy(cfg.description, port_config.description.c_str(), sizeof(cfg.description));
        cfg.description[sizeof(cfg.description) - 1] = '\0';
        return;
      }
    }
    // Take first free row
    for (auto& cfg : config->config_list) {
      if (!cfg.used) {
        cfg.used = true;
        cfg.port = port_config.port;

        strncpy(cfg.name, port_config.name.c_str(), sizeof(cfg.name));
        cfg.name[sizeof(cfg.name) - 1] = '\0';

        strncpy(cfg.share_name, port_config.share_name.c_str(), sizeof(cfg.share_name));
        cfg.share_name[sizeof(cfg.share_name) - 1] = '\0';

        strncpy(cfg.description, port_config.description.c_str(), sizeof(cfg.description));
        cfg.description[sizeof(cfg.description) - 1] = '\0';
        break;
      }
    }
  } catch (const std::exception& error) {
    LOG_INFO() << "Failed to connect to the shared memory. Error: " << error.what();
  }

}

void DeleteListenConfig(uint16_t port) {
  if (port == 0) {
    return;
  }

  try {
    shared_memory_object shared_mem(open_only, kShareMemName.data(), read_write);
    mapped_region region(shared_mem, read_write);
    auto* config = static_cast<ShareListenConfig*> (region.get_address());
    scoped_lock lock(config->locker);

    // First check if port exist. If so update the configuration
    for (auto& cfg : config->config_list) {
      if (cfg.used && port == cfg.port) {
        cfg.used = false;
        cfg.port = 0;
        memset(cfg.name, '\0', sizeof(cfg.name));
        memset(cfg.share_name, '\0', sizeof(cfg.share_name));
        memset(cfg.description, '\0', sizeof(cfg.description));
      }
    }
  } catch (const std::exception& error) {
    LOG_INFO() << "Failed to connect to the shared memory. Error: " << error.what();
  }

}

}