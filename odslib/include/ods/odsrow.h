/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>
#include <vector>
#include <ods/odsitem.h>

namespace ods {
class OdsRow {
 public:
  explicit OdsRow(const std::string& app_name);

  int64_t ApplicationId() const {
    return application_id_;
  }
  void ApplicationId(int64_t ident) {
    application_id_ = ident;
  }
  [[nodiscard]] const std::string& ApplicationName() const {
    return application_name_;
  }
  void ApplicationName(const std::string& name) {
    application_name_ = name;
  }

  void AddItem(const OdsItem& item) {
    item_list_.push_back(item);
  }
  [[nodiscard]] const OdsItem* GetItem(const std::string& name) const;
  [[nodiscard]] const std::vector<OdsItem>& ItemList() const {
    return item_list_;
  }

  [[nodiscard]] std::vector<OdsItem>& ItemList() {
    return item_list_;
  }

 private:
  int64_t     application_id_ = 0;
  std::string application_name_;
  std::vector<OdsItem> item_list_;
};

}




