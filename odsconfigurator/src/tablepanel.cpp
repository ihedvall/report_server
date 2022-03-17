/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <wx/docmdi.h>
#include <wx/splitter.h>
#include "tablepanel.h"
#include "odsconfigid.h"
namespace {

int BaseIdImage(ods::BaseId base_id) {
  switch (base_id) {
    case ods::BaseId::AoAny: return 0;
    case ods::BaseId::AoEnvironment: return 1;
    case ods::BaseId::AoSubTest: return 3;
    case ods::BaseId::AoMeasurement: return 4;
    case ods::BaseId::AoMeasurementQuantity: return 8;
    case ods::BaseId::AoQuantity: return 9;
    case ods::BaseId::AoQuantityGroup: return 10;
    case ods::BaseId::AoUnit: return 11;
    case ods::BaseId::AoUnitGroup: return 12;
    case ods::BaseId::AoPhysicalDimension: return 13;
    case ods::BaseId::AoUnitUnderTest: return 14;
    case ods::BaseId::AoUnitUnderTestPart: return 15;
    case ods::BaseId::AoTestEquipment: return 16;
    case ods::BaseId::AoTestEquipmentPart: return 17;
    case ods::BaseId::AoTestSequence: return 18;
    case ods::BaseId::AoTestSequencePart: return 19;
    case ods::BaseId::AoUser: return 20;
    case ods::BaseId::AoUserGroup: return 21;
    case ods::BaseId::AoTest: return 2;
    case ods::BaseId::AoTestDevice: return 17;
    case ods::BaseId::AoSubMatrix: return 5;
    case ods::BaseId::AoLocalColumn: return 6;
    case ods::BaseId::AoExternalComponent: return 7;
    case ods::BaseId::AoLog: return 22;
    case ods::BaseId::AoParameter: return 23;
    case ods::BaseId::AoParameterSet: return 24;
    case ods::BaseId::AoNameMap: return 26;
    case ods::BaseId::AoAttributeMap: return 27;
    case ods::BaseId::AoFile: return 28;
    case ods::BaseId::AoView: return 0;
    default:break;
  }
  return 30;
}

  void AppendSubTable(wxTreeListCtrl& tree_list, wxTreeListItem& root, const ods::ITable& table) {
    const auto& table_list = table.SubTables();
    // Environment table is always the root. It's OK if it is missing, so we don't show it.
    for ( const auto& itr : table_list) {
      const auto& sub_table = itr.second;
      const auto bmp = BaseIdImage(sub_table.BaseId());
      auto item_root = tree_list.AppendItem(root,sub_table.ApplicationName(), bmp, bmp);
      tree_list.SetItemText(item_root, 1, BaseIdToText(sub_table.BaseId()));
      AppendSubTable(tree_list, item_root, sub_table);
    }
  }
} // end namespace

namespace ods::gui {

wxBEGIN_EVENT_TABLE(TablePanel, wxPanel)
        EVT_TREELIST_SELECTION_CHANGED(kIdTableList, TablePanel::OnTableSelect)
wxEND_EVENT_TABLE()

TablePanel::TablePanel(wxWindow *parent)
    : wxPanel(parent),
      image_list_(16, 16, false, 31) {
  image_list_.Add(wxBitmap("TREE_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  auto *splitter = new wxSplitterWindow(this);
  left_ = new wxTreeListCtrl(splitter, kIdTableList, wxDefaultPosition, {350, 600},
                             wxTL_SINGLE | wxTL_DEFAULT_STYLE);
  left_->AppendColumn("Name");
  left_->AppendColumn("Base Id");
  left_->SetImageList(&image_list_);

  auto* right_panel = new wxPanel(splitter);
  right_ = new wxListView(right_panel, kIdColumnList, wxDefaultPosition, {850, 600}, wxLC_REPORT);
  right_->AppendColumn("Flags", wxLIST_FORMAT_LEFT, 50);
  right_->AppendColumn("Name", wxLIST_FORMAT_LEFT, 100);
  right_->AppendColumn("DB", wxLIST_FORMAT_LEFT, 100);
  right_->AppendColumn("Label", wxLIST_FORMAT_LEFT, 150);
  right_->AppendColumn("Type", wxLIST_FORMAT_LEFT,200);
  right_->AppendColumn("Reference", wxLIST_FORMAT_LEFT, 100);
  right_->AppendColumn("Description", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
  table_info_ = new wxStaticText(right_panel,wxID_ANY, wxEmptyString);
  auto font = table_info_->GetFont();
  font.MakeLarger();
  table_info_->SetFont(font);

  splitter->SplitVertically(left_, right_panel, 300);

  auto* right_sizer = new wxBoxSizer(wxVERTICAL);
  right_sizer->Add(table_info_, 0, wxALIGN_LEFT | wxALL | wxEXPAND , 0);
  right_sizer->Add(right_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  right_panel->SetSizerAndFit(right_sizer);

  auto *main_sizer = new wxBoxSizer(wxHORIZONTAL);
  main_sizer->Add(splitter, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  SetSizerAndFit(main_sizer);

  RedrawTableList();
  RedrawColumnList();
}

void TablePanel::RedrawTableList() {
  auto *doc = GetDocument();
  if (doc == nullptr || left_ == nullptr) {
    return;
  }

  std::string selected_table;
  const auto item = left_->GetSelection();
  if (item.IsOk()) {
    selected_table = left_->GetItemText(item).ToStdString();
  }

  left_->DeleteAllItems();
  const auto &model = doc->GetModel();
  const auto& table_list = model.Tables();

  constexpr std::array<BaseId,8> load_order = {BaseId::AoEnvironment,
                                               BaseId::AoTest,
                                               BaseId::AoUnitUnderTest,
                                               BaseId::AoTestSequence,
                                               BaseId::AoTestEquipment,
                                               BaseId::AoUnit,
                                               BaseId::AoPhysicalDimension,
                                               BaseId::AoQuantity };
  const auto root = left_->GetRootItem();
  for (const auto load : load_order) {
    for (const auto &itr: table_list) {
      const auto &table = itr.second;
      if (table.BaseId() != load) {
        continue;
      }
      const auto bmp = BaseIdImage(table.BaseId());
      auto item_root = left_->AppendItem(root, table.ApplicationName(), bmp, bmp);
      left_->SetItemText(item_root, 1, BaseIdToText(table.BaseId()));
      AppendSubTable(*left_, item_root, table);
    }
  }

  for (const auto &itr1: table_list) {
    const auto &table = itr1.second;
    const auto ignore = std::ranges::any_of(load_order, [&] (BaseId base) {
      return base == table.BaseId();
    });
    if (ignore) {
      continue;
    }
    const auto bmp = BaseIdImage(table.BaseId());
    auto item_root = left_->AppendItem(root, table.ApplicationName(), bmp, bmp);
    left_->SetItemText(item_root, 1, BaseIdToText(table.BaseId()));
    AppendSubTable(*left_, item_root, table);
  }

}

void TablePanel::RedrawColumnList() {
  auto *doc = GetDocument();
  if (doc == nullptr || left_ == nullptr || right_ == nullptr || table_info_ == nullptr) {
    return;
  }
  right_->DeleteAllItems();
  const auto selected = left_->GetSelection();

  if (!selected.IsOk()) {
    table_info_->SetLabel("");
    return;
  }

  const auto name = left_->GetItemText(selected);
  const auto& model = doc->GetModel();
  const auto* table = model.GetTableByName(name.ToStdString());
  if (table == nullptr) {
   return;
  }
  std::ostringstream info;
  info << table->ApplicationName() << " (" << table->ApplicationId() << ")";
  if (!table->DatabaseName().empty()) {
    info << ", DB: " << table->DatabaseName();
  }
  if (!table->Description().empty()) {
    info << ", Desc: " << table->Description();
  }
  table_info_->SetLabel(info.str());

  const auto& column_list = table->Columns();
  long row = 0;
  for (const auto& column : column_list) {
    std::ostringstream flags;
    if (column.Flags() & kUnique) {
      flags << "U";
    }
    if (column.Flags() & kObligatory) {
      flags << "O";
    }
    if (column.Flags() & kAutoGenerated) {
      flags << "A";
    }
    if (column.Flags() & kCaseSensitive) {
      flags << "C";
    }
    if (column.Flags() & kIndex) {
      flags << "I";
    }

    std::ostringstream type;
    type << DataTypeToUserText(column.DataType());
    if (column.DataType() == DataType::DtEnum) {
      type << " (" << column.EnumName() << ")";
    }

    if (column.DataType() == DataType::DtFloat || column.DataType() == DataType::DtDouble) {
      type << " :" << column.NofDecimals();
    }

    if (column.DataLength() > 0) {
      type << " (" << column.DataLength() << ")";
    }
    if (!column.Unit().empty()) {
      type << " [" << column.Unit() << "]";
    }

    std::ostringstream ref;
    if (column.ReferenceId() > 0) {
      const auto* ref_table = model.GetTable(column.ReferenceId());
      if (ref_table != nullptr) {
        ref << ref_table->ApplicationName();
        const auto* ref_column = column.ReferenceName().empty() ?
            nullptr : ref_table->GetColumnByName(column.ReferenceName());
        if (ref_column != nullptr && !util::string::IEquals("id",ref_column->BaseName())) {
          ref << " (" << ref_column->ApplicationName() << ")";
        }
      }
    }


    right_->InsertItem(row, flags.str());
    right_->SetItem(row, 1, wxString::FromUTF8(column.ApplicationName()));
    right_->SetItem(row, 2, wxString::FromUTF8(column.DatabaseName()));
    right_->SetItem(row, 3, wxString::FromUTF8(column.DisplayName()));
    right_->SetItem(row, 4, wxString::FromUTF8(type.str()));
    right_->SetItem(row, 5, wxString::FromUTF8(ref.str()));
    right_->SetItem(row, 6, wxString::FromUTF8(column.Description()));
    ++row;
  }
}

OdsDocument *TablePanel::GetDocument() const {
  const auto *child_frame = wxDynamicCast(GetGrandParent(), wxDocMDIChildFrame); // NOLINT
  return child_frame != nullptr ? wxDynamicCast(child_frame->GetDocument(), OdsDocument) : nullptr; //NOLINT
}

void TablePanel::Update() {
  RedrawTableList();
  RedrawColumnList();
  wxWindow::Update();
}

void TablePanel::OnTableSelect(wxTreeListEvent &) {
  RedrawColumnList();
}

}