/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include "iblock.h"
#include "dg4block.h"
#include "fh4block.h"
#include "ch4block.h"
#include "at4block.h"
#include "ev4block.h"
#include "md4block.h"
#include "mdf4timestamp.h"

namespace mdf::detail {

enum class Hd4TimeClass : uint8_t {
  LocalPcTime = 0,
  ExternalTime = 10,
  ExternalNTP = 16
};

namespace Hd4Flags {
  constexpr uint8_t kStartAngleValid = 0x01;
  constexpr uint8_t kStartDistanceValid = 0x02;
}

class Hd4Block : public IBlock {
 public:
  using Dg4List = std::vector<std::unique_ptr<Dg4Block>>;
  using Fh4List = std::vector<std::unique_ptr<Fh4Block>>;
  using Ch4List = std::vector<std::unique_ptr<Ch4Block>>;
  using At4List = std::vector<std::unique_ptr<At4Block>>;
  using Ev4List = std::vector<std::unique_ptr<Ev4Block>>;

  [[nodiscard]] const Dg4List &Dg4() const {
    return dg_list_;
  }

  [[nodiscard]] const Fh4List &Fh4() const {
    return fh_list_;
  }

  [[nodiscard]] const Ch4List &Ch4() const {
    return ch_list_;
  }

  [[nodiscard]] const At4List &At4() const {
    return at_list_;
  }

  [[nodiscard]] const Ev4List &Ev4() const {
    return ev_list_;
  }

  [[nodiscard]] const IBlock* Find(fpos_t index) const override;
  void GetBlockProperty(BlockPropertyList &dest) const override;

  size_t Read(std::FILE *file) override;
  void ReadMeasurementInfo(std::FILE *file);
  void ReadEverythingButData(std::FILE *file);

 private:
  Mdf4Timestamp timestamp_;

  uint8_t time_class_ = 0;
  uint8_t flags_ = 0;
  /* 1 byte reserved */;
  double start_angle_ = 0;    ///< Unit is radians.
  double start_distance_ = 0; ///< Unit is meters.

  Dg4List dg_list_;
  Fh4List fh_list_;
  Ch4List ch_list_;
  At4List at_list_;
  Ev4List ev_list_;

};
}


