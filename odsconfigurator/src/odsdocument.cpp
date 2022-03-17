/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <sstream>
#include <filesystem>
#include <wx/msgdlg.h>
#include "util/logstream.h"
#include "odsdocument.h"

using namespace util::log;

namespace {

} // end namespace

namespace ods::gui {
wxIMPLEMENT_DYNAMIC_CLASS(OdsDocument, wxDocument) // NOLINT

wxBEGIN_EVENT_TABLE(OdsDocument, wxDocument) // NOLINT
  EVT_UPDATE_UI(wxID_SAVE, OdsDocument::OnUpdateSave)
wxEND_EVENT_TABLE()

bool OdsDocument::OnOpenDocument(const wxString &filename) {
  wxBusyCursor wait;
  const auto read = model_.ReadModel(filename.ToStdString());
  if (!read) {
    return false;
  }
  Modify(false);
  return wxDocument::OnOpenDocument(filename);
}

void OdsDocument::OnUpdateSave(wxUpdateUIEvent &event) {
  try {
    std::filesystem::path file(GetFilename().ToStdString());
    if (!std::filesystem::exists(file)) {
      event.Enable(false);
      return;
    }
  } catch (const std::exception& ) {
    event.Enable(false);
    return;
  }
  event.Enable(IsModified());
}

bool OdsDocument::IsModified() const {
  return !(original_ == model_);
}

OdsDocument::OdsDocument() {
  model_ = original_;
}

void OdsDocument::Modify(bool mod) {
  if (!mod) {
    original_ = model_;
  }
  wxDocument::Modify(mod);
}

} // namespace mdf::viewer