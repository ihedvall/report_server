/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdio>
#include <string>
#include <memory>
#include <vector>
#include "mdf/iheader.h"
#include "mdf/iattachment.h"
#include "mdf/idatagroup.h"

namespace mdf {
using AttachmentList = std::vector<const IAttachment*>;
using DataGroupList = std::vector<const IDataGroup*>;
class MdfFile {
 public:
  virtual ~MdfFile() = default;

  virtual void Attachments(AttachmentList& dest) const = 0;
  virtual void DataGroups(DataGroupList& dest ) const = 0;

  [[nodiscard]] virtual bool IsMdf4() const = 0;

  virtual void ReadHeader(std::FILE *file) = 0;
  virtual void ReadMeasurementInfo(std::FILE *file) = 0;
  virtual void ReadEverythingButData(std::FILE *file) = 0;

 protected:
  MdfFile() = default;
};

}



