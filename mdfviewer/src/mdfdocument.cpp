/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <sstream>

#include <filesystem>

#ifndef __kernel_entry
#define __kernel_entry
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include <boost/process.hpp>
#pragma GCC diagnostic pop
#include <boost/process/windows.hpp>
#include <boost/filesystem.hpp>

#include <wx/msgdlg.h>

#include "util/logstream.h"
#include "util/csvwriter.h"
#include "mdfdocument.h"
#include "windowid.h"
#include "mdfviewer.h"
#include "channelobserverframe.h"

using namespace util::log;
using namespace mdf;
namespace {
/**
 * Creates a CSV file using sample observers as input.
 *
 * The CSV file is created in the applications temporary directory and the file name is unique.
 * Note that if the observer list only have one channel, the file will add the samples as first column.
 * The same applies if the first channel not is a master channel.
 * @param list Sample observer list containing samples.
 * @return Full path to the created file or an empty string if the function fails.
 */
std::string CreateCsvFile(const mdf::ChannelObserverList& list) {
  if (list.empty()) {
    LOG_ERROR() << "No observers in the list.";
    return "";
  }

  const auto& app = mdf::viewer::wxGetApp();
  std::string csv_file;
  try {
    std::filesystem::path temp_file(app.GetMyTempDir());
    const auto unique  = boost::filesystem::unique_path();
    temp_file.append(unique.string());
    temp_file.replace_extension(".csv");
    csv_file = temp_file.generic_string();
  } catch (const std::exception& error) {
    LOG_ERROR() << "Failed to create the CSV file. Error: " << error.what();
  }

 util::string::CsvWriter csv(csv_file);

  const bool master = list.size() > 1 && list[0]->IsMaster();

  // Add the header first
  if (!master) {
    // First column is sample number
    csv.AddColumnHeader("Sample");
  }
  for (const auto& channel : list) {
     std::ostringstream s;
     s << channel->Name();
     if (!channel->Unit().empty()) {
       s << " [" << channel->Unit() << "]";
     }
     csv.AddColumnHeader(s.str());
  }
  csv.AddRow();
  // Add the samples
  for (size_t sample = 0; sample < list[0]->NofSamples(); ++sample) {
    // Add the sample number first
    if (!master) {
      // First column is sample number
      csv.AddColumnValue(sample);
    }
    for (const auto& channel : list) {
      std::string value;
      const bool valid  = channel->GetEngValue(sample, value); // Note that we may lose some digits here
      csv.AddColumnValue(valid ? value : "");
    }
    csv.AddRow();
  }

 return csv_file;
}

std::string CreateGnuPlotFile(const mdf::ChannelObserverList& list, const std::string& csv_file, const std::string& title) {
  if (list.empty() || csv_file.empty()) {
    LOG_ERROR() << "No CSV file or observer list is empty.";
    return "";
  }

  std::string gp_file;
  try {
    std::filesystem::path p(csv_file);
    p.replace_extension(".gp");
    gp_file = p.generic_string();
  } catch(const std::exception& error) {
    LOG_ERROR() << "Failed to create gnuplot file. CSV File: " << csv_file;
  }
  const bool master = list.size() > 1 && list[0]->IsMaster();
  std::FILE* file = fopen(gp_file.c_str(), "wt");
  std::ostringstream x_label;
  std::ostringstream y_label;
  if (!master) {
    x_label << "Sample";
    y_label << list[0]->Name();
    if (!list[0]->Unit().empty()) {
      y_label << " [" << list[0]->Unit() << "]";
    }
  } else {
    x_label << list[0]->Name();
    if (!list[0]->Unit().empty()) {
      x_label << " [" << list[0]->Unit() << "]";
    }

    y_label << list[1]->Name();
    if (!list[1]->Unit().empty()) {
      y_label << " [" << list[1]->Unit() << "]";
    }
  }

  if (file != nullptr) {
    fprintf(file, "set terminal wxt noenhanced title \"%s\" size 1200, 800\n", title.c_str());
    fprintf(file, "set datafile separator comma\n");
    fprintf(file, "set key outside autotitle columnheader\n");
    fprintf(file, "set xlabel \"%s\"\n", x_label.str().c_str());
    fprintf(file, "set ylabel \"%s\"\n", y_label.str().c_str());
    fprintf(file, "set autoscale\n");
    fprintf(file, "plot \"%s\" using 1:2 with lines\n",csv_file.c_str());
    fprintf(file, "exit\n");
    fclose(file);
  }
  return gp_file;
}

} // end namespace

namespace mdf::viewer {
wxIMPLEMENT_DYNAMIC_CLASS(MdfDocument, wxDocument) // NOLINT

wxBEGIN_EVENT_TABLE(MdfDocument, wxDocument) // NOLINT
  EVT_UPDATE_UI(kIdSaveAttachment, MdfDocument::OnUpdateSaveAttachment)
  EVT_MENU(kIdSaveAttachment, MdfDocument::OnSaveAttachment)
  EVT_UPDATE_UI(kIdShowGroupData, MdfDocument::OnUpdateShowGroupData)
  EVT_MENU(kIdShowGroupData, MdfDocument::OnShowGroupData)
  EVT_UPDATE_UI(kIdShowChannelData, MdfDocument::OnUpdateShowChannelData)
  EVT_MENU(kIdShowChannelData, MdfDocument::OnShowChannelData)
  EVT_UPDATE_UI(kIdPlotChannelData, MdfDocument::OnUpdatePlotChannelData)
  EVT_MENU(kIdPlotChannelData, MdfDocument::OnPlotChannelData)
wxEND_EVENT_TABLE()

bool MdfDocument::OnOpenDocument(const wxString &filename) {

  wxBusyCursor wait;
  reader_ = std::make_unique<MdfReader>(filename.ToStdString()); // Note that the file is now open
  bool parse = reader_->IsOk();
  if (!parse) {
    LOG_ERROR() << "The file is not an MDF file. File: "
                << filename;
    wxMessageBox(wxString("File is not an MDF file"),
                 wxString("Error Open File"),
                 wxOK | wxCENTRE | wxICON_ERROR);
    reader_->Close();
    return false;
  }

  try {
    parse = reader_->ReadEverythingButData();
   } catch (const std::exception &error) {
    LOG_ERROR() << "Parsing error. Error: " << error.what()
                << ", File: " << filename;
    parse = false;
  }
  reader_->Close();
  if (!parse) {
    wxMessageBox(wxString("Failed to parse the file.\nMore information in the log file."),
                 wxString("Error Parsing File"),
                 wxOK | wxCENTRE | wxICON_ERROR);
  }
  return parse && wxDocument::OnOpenDocument(filename);
}

const mdf::detail::IBlock *MdfDocument::GetBlock(fpos_t id) const {
  const auto *file = GetFile();
  if (file == nullptr || id < 0) {
    return nullptr;
  }

  const mdf::detail::IBlock *block = nullptr;
  if (file->IsMdf4()) {
    const auto *file4 = dynamic_cast<const mdf::detail::Mdf4File *>(file);
    if (file4 != nullptr) {
      block = file4->Find(id);
    }
  } else {
    const auto *file3 = dynamic_cast<const mdf::detail::Mdf3File *>(file);
    if (file3 != nullptr) {
      block = file3->Find(id);
    }
  }
  return block;
}

void MdfDocument::OnSaveAttachment(wxCommandEvent &event) {
  const auto id = GetSelectedBlockId();
  const auto *block = GetBlock(id);
  if (block == nullptr) {
    return;
  }
  const auto* at4 = dynamic_cast<const mdf::detail::At4Block*>(block);
  if (at4 == nullptr) {
    return;
  }

  wxFileDialog save_dialog(wxGetActiveWindow(), "Select the file name for the attachment",
                           "",
                           at4->FileName(),
                           "All Files (*.*)|*.*",
                           wxFD_OVERWRITE_PROMPT | wxFD_SAVE);
  auto ret = save_dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  std::string filename = save_dialog.GetPath().ToStdString();
  const auto ok = reader_->ReadAttachmentData(*at4,filename);
  if (!ok) {
    wxMessageBox("Failed to create the file.\nMore information in the log file.",
                 "File Save Error",
                 wxOK | wxCENTRE | wxICON_ERROR);
    return;
  }

  std::ostringstream open;
  open << "Do you want to open the file ?" << std::endl
    << "File: " << filename;
  auto o = wxMessageBox(open.str(),"Open File",
                 wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION);
  if (o == wxYES) {
    auto& app = wxGetApp();
    app.OpenFile(filename);
  }
}

void MdfDocument::OnUpdateSaveAttachment(wxUpdateUIEvent &event) {
  if (!reader_) {
    event.Enable(false);
    return;
  }
  const auto id = GetSelectedBlockId();
  const auto *block = GetBlock(id);
  if (block == nullptr) {
    event.Enable(false);
    return;
  }
  event.Enable(block->BlockType() == "AT");
}

void MdfDocument::OnShowGroupData(wxCommandEvent &event) {
  const auto selected_id = GetSelectedBlockId();
  const auto *selected_block = GetBlock(selected_id);
  if (selected_block == nullptr) {
    return;
  }
  const IDataGroup* dg = nullptr;
  const IChannelGroup* cg = nullptr;
  if (selected_block->BlockType() == "DG") {
    dg = dynamic_cast<const IDataGroup*>(selected_block);
    if (dg != nullptr) {
     auto cg_list = dg->ChannelGroups();
     if (!cg_list.empty()) {
       cg = cg_list[0];
     }
    }
  } else if (selected_block->BlockType() == "CG") {
    cg = dynamic_cast<const IChannelGroup*>(selected_block);
    const auto parent_id = GetParentBlockId();
    const auto *parent_block = GetBlock(parent_id);
    if (parent_block != nullptr) {
      dg = dynamic_cast<const IDataGroup*>(parent_block);
    }
  }
  if (dg == nullptr || cg == nullptr) {
    return;
  }

  auto* view = GetFirstView();
  if (view == nullptr) {
    return;
  }

  auto* parent = view->GetFrame();
  if (parent == nullptr) {
    return;
  }

  std::ostringstream title;
  if (!cg->Name().empty()) {
    title << cg->Name();
  }

  if (!dg->Description().empty()) {
    if (!title.str().empty()) {
      title << "/";
    }
    title << dg->Description();
  }

  if (!title.str().empty()) {
    title << "/";
  }
  title << reader_->ShortName();

  auto observer_list = std::make_unique<mdf::ChannelObserverList>();
  mdf::CreateChannelObserverForChannelGroup(*dg, *cg, *observer_list);
  const bool read = reader_->ReadData(*dg);
  if (!read) {
    wxMessageBox("The read failed.\nMore information in the log file.",
                 "Failed to Read Data Block", wxCENTRE | wxICON_ERROR);
    return;
  }
  auto data_frame = new ChannelObserverFrame(observer_list, parent, wxID_ANY, title.str());
  data_frame->Show();
}

void MdfDocument::OnUpdateShowGroupData(wxUpdateUIEvent &event) {
  event.Enable(false);
  if (!reader_) {
    return;
  }

  const auto selected_id = GetSelectedBlockId();
  const auto selected_block = GetBlock(selected_id);
  if (selected_block == nullptr) {
    return;
  }

  if (selected_block->BlockType() == "DG") {
    const auto* dg = dynamic_cast<const IDataGroup*>(selected_block);
    if (dg == nullptr) {
      return;
    }
    auto cg_list = dg->ChannelGroups();
    event.Enable(cg_list.size() == 1);
  } else if (selected_block->BlockType() == "CG") {
    event.Enable(true);
  }
}

void MdfDocument::OnShowChannelData(wxCommandEvent &event) {
  const IDataGroup* dg = nullptr;
  const IChannelGroup* cg = nullptr;
  const IChannel* cn = nullptr;

  const auto selected_id = GetSelectedBlockId();
  const auto *selected_block = GetBlock(selected_id);
  if (selected_block != nullptr && selected_block->BlockType() == "CN") {
    cn = dynamic_cast<const IChannel*>(selected_block);
  }

  const auto parent_id = GetParentBlockId();
  const auto *parent_block = GetBlock(parent_id);
  if (parent_block != nullptr && parent_block->BlockType() == "CG") {
    cg = dynamic_cast<const IChannelGroup*>(parent_block);
  }

  const auto grand_parent_id = GetGrandParentBlockId();
  const auto *grand_parent_block = GetBlock(grand_parent_id);
  if (grand_parent_block != nullptr && grand_parent_block->BlockType() == "DG") {
    dg = dynamic_cast<const IDataGroup*>(grand_parent_block);
  }

  if (dg == nullptr || cg == nullptr || cn == nullptr) {
    return;
  }
  const auto* x_axis = cg->GetXChannel(*cn); // Need to show the master channel as well as the data channel

  auto* view = GetFirstView();
  if (view == nullptr) {
    return;
  }

  auto* parent = view->GetFrame();
  if (parent == nullptr) {
    return;
  }

  std::ostringstream title;
  if (!cn->Name().empty()) {
    title << cn->Name();
  }

  if (!cg->Name().empty()) {
    if (!title.str().empty()) {
      title << "/";
    }
    title << cg->Name();
  }

  if (!dg->Description().empty()) {
    if (!title.str().empty()) {
      title << "/";
    }
    title << dg->Description();
  }

  if (!title.str().empty()) {
    title << "/";
  }
  title << reader_->ShortName();

  auto observer_list = std::make_unique<mdf::ChannelObserverList>();
  if (x_axis != nullptr && x_axis->Index() != cn->Index()) {
    observer_list->emplace_back(std::move(mdf::CreateChannelObserver(*dg, *cg, *x_axis)));
  }
  observer_list->emplace_back(std::move(mdf::CreateChannelObserver(*dg, *cg, *cn)));

  const bool read = reader_->ReadData(*dg);
  if (!read) {
    wxMessageBox("The read failed.\nMore information in the log file.",
                 "Failed to Read Data Block", wxCENTRE | wxICON_ERROR);
    return;
  }

  auto data_frame = new ChannelObserverFrame(observer_list, parent, wxID_ANY, title.str());
  data_frame->Show();
}

void MdfDocument::OnUpdateShowChannelData(wxUpdateUIEvent &event) {
  event.Enable(false);
  if (!reader_) {
    return;
  }

  const auto selected_id = GetSelectedBlockId();
  const auto selected_block = GetBlock(selected_id);
  if (selected_block == nullptr) {
    return;
  }

  if (selected_block->BlockType() == "CN") {
    event.Enable(true);
  }
}

void MdfDocument::OnPlotChannelData(wxCommandEvent &event) {
  const IDataGroup* dg = nullptr;
  const IChannelGroup* cg = nullptr;
  const IChannel* cn = nullptr;

  const auto selected_id = GetSelectedBlockId();
  const auto *selected_block = GetBlock(selected_id);
  if (selected_block != nullptr && selected_block->BlockType() == "CN") {
    cn = dynamic_cast<const IChannel*>(selected_block);
  }

  const auto parent_id = GetParentBlockId();
  const auto *parent_block = GetBlock(parent_id);
  if (parent_block != nullptr && parent_block->BlockType() == "CG") {
    cg = dynamic_cast<const IChannelGroup*>(parent_block);
  }

  const auto grand_parent_id = GetGrandParentBlockId();
  const auto *grand_parent_block = GetBlock(grand_parent_id);
  if (grand_parent_block != nullptr && grand_parent_block->BlockType() == "DG") {
    dg = dynamic_cast<const IDataGroup*>(grand_parent_block);
  }
  auto& app = wxGetApp();
  if (dg == nullptr || cg == nullptr || cn == nullptr || app.GnuPlot().empty()) {
    return;
  }
  const auto* x_axis = cg->GetXChannel(*cn); // Need to show the master channel as well as the data channel

  std::ostringstream title;
  if (!cn->Name().empty()) {
    title << cn->Name();
  }

  if (!cg->Name().empty()) {
    if (!title.str().empty()) {
      title << "/";
    }
    title << cg->Name();
  }

  if (!dg->Description().empty()) {
    if (!title.str().empty()) {
      title << "/";
    }
    title << dg->Description();
  }

  if (!title.str().empty()) {
    title << "/";
  }
  title << reader_->ShortName();

  // Create the observer list
  auto observer_list = std::make_unique<mdf::ChannelObserverList>();
  if (x_axis != nullptr) {
    observer_list->emplace_back(std::move(mdf::CreateChannelObserver(*dg, *cg, *x_axis)));
  }
  observer_list->emplace_back(std::move(mdf::CreateChannelObserver(*dg, *cg, *cn)));

  // Read in the values from the MDF file
  const bool read = reader_->ReadData(*dg);
  if (!read) {
    wxMessageBox("The read failed.\nMore information in the log file.",
                 "Failed to Read Data Block", wxCENTRE | wxICON_ERROR);
    return;
  }

  // Produce a CSV file with the data for later use with the gnuplot script
  auto csv_file = CreateCsvFile(*observer_list);
  auto gp_file = CreateGnuPlotFile(*observer_list, csv_file, title.str());
  if (csv_file.empty() || gp_file.empty()) {
    wxMessageBox("Failed to create CSV or GP files.\nMore information in log file.");
    return;
  }
  boost::process::spawn(app.GnuPlot(), "--persist", gp_file, boost::process::windows::hide);
}

void MdfDocument::OnUpdatePlotChannelData(wxUpdateUIEvent &event) {
  event.Enable(false);
  auto& app = wxGetApp();
  if (!reader_ || app.GnuPlot().empty()) {
    return;
  }

  const auto selected_id = GetSelectedBlockId();
  const auto selected_block = GetBlock(selected_id);
  if (selected_block == nullptr) {
    return;
  }

  if (selected_block->BlockType() != "CN") {
    return;
  }
  const auto* cn = dynamic_cast<const IChannel*>(selected_block);
  if (cn == nullptr) {
    return;
  }

  // Note that plotting of strings and absolute times are tricky.
  // The timestamp (CAN Open Date/Time) is possible but labeling the tics is
  // almost impossible.
  if (!cn->IsNumber()) {
    return;
  }
  event.Enable(true);
}

} // namespace mdf::viewer