/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace util::log::detail {

struct ListenPortConfig {
  uint16_t port;
  std::string name;
  std::string share_name;
  std::string description;
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




