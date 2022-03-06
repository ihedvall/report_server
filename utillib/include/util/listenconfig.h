/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace util::log {

struct ListenPortConfig {
  uint16_t port;
  std::string name;
  std::string share_name;
  std::string description;
  bool operator < (ListenPortConfig& listen) const {
    return name < listen.name;
  }
};

[[nodiscard]] std::vector<ListenPortConfig> GetListenConfigList();
void AddListenConfig(const ListenPortConfig& port_config);
void DeleteListenConfig(uint16_t port);

class ListenConfig final {
 public:
  ListenConfig();
  ~ListenConfig();
};

} // end namespace




