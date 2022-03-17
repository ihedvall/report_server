/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <memory>
#include <wx/wx.h>
#include <wx/docview.h>
#include "ods/imodel.h"

namespace ods::gui {

class OdsDocument : public wxDocument {
 public:
  OdsDocument();
  ~OdsDocument() override = default;

  bool OnOpenDocument(const wxString &filename) override;

  IModel& GetModel() {
    return model_;
  }
 private:
  ods::IModel original_;
  ods::IModel model_;
  void OnUpdateSave(wxUpdateUIEvent &event);
  wxDECLARE_DYNAMIC_CLASS(OdsDocument);
  [[nodiscard]] bool IsModified() const override;
  void Modify(bool mod) override;
 wxDECLARE_EVENT_TABLE();
};



}



