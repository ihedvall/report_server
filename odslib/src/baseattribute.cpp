/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string_view>
#include <array>
#include <algorithm>
#include "ods/baseattribute.h"
#include "util/stringutil.h"
using namespace ods;
using namespace util::string;

namespace {
  struct BaseAttrDef {
    BaseId BelongsTo = BaseId::AoNotDefined;
    bool Mandatory = false;
    const std::string_view BaseName;
    const std::string_view AppName;
    const std::string_view DbName;
    const std::string_view DisplayName;
    DataType Type = DataType::DtFloat;
    uint16_t Flags = 0;
  };

  constexpr std::array<BaseAttrDef, 116> kBaseAttrList = {
      // BASIC
      BaseAttrDef{BaseId::AoNotDefined, true, "id", "Index", "IID", "Item ID", DataType::DtLongLong, kAutoGenerated | kUnique | kIndex},
      BaseAttrDef{BaseId::AoNotDefined, true, "name", "Name", "NAME", "Name", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoNotDefined, false, "description", "Description", "DESCRIPTION", "Description", DataType::DtString,0},
      BaseAttrDef{BaseId::AoNotDefined, false, "version", "Version", "VERSION", "Version", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoNotDefined, false, "version_date", "Stored", "STORED", "Stored", DataType::DtDate,0},
      BaseAttrDef{BaseId::AoNotDefined, false, "mime_type", "MimeType", "MIME_TYPE", "MIME Type", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoNotDefined, false, "objecttype", "ObjectId", "OBJECT_ID","Object ID", DataType::DtLongLong, 0},
      BaseAttrDef{BaseId::AoNotDefined, false, "ao_created", "Created", "CREATED", "Created", DataType::DtDate, kAutoGenerated},
      BaseAttrDef{BaseId::AoNotDefined, false, "ao_created_by", "CreatedBy", "CREATED_BY", "Created By", DataType::DtString,kAutoGenerated},
      BaseAttrDef{BaseId::AoNotDefined, false, "ao_last_modified", "Modified", "MODIFIED", "Modified", DataType::DtDate, kAutoGenerated},
      BaseAttrDef{BaseId::AoNotDefined, false, "ao_last_modified_by", "ModifiedBy", "MODIFIED_BY", "Modified By", DataType::DtString, kAutoGenerated},
      // ENVIRONMENT
      BaseAttrDef{BaseId::AoEnvironment, false, "max_test_level", "TestLevel", "TEST_LEVEL", "Max Test Level", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoEnvironment, false, "base_model_version", "BaseModelVersion", "BASE_MODEL", "Base Model Version", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoEnvironment, false, "application_model_type", "AppType", "APP_TYPE", "Application Type", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoEnvironment, false, "application_model_version", "AppVersion", "APP_VERSION", "Application Version", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoEnvironment, false, "timezone", "TimeZone", "TIMEZONE", "Timezone", DataType::DtString, 0},
      // NAME MAP
      BaseAttrDef{BaseId::AoNameMap, true, "entity_name", "EntityName", "ENTITY_NAME", "Entity Name", DataType::DtString, 0},
      // ATTRIBUTE MAP
      BaseAttrDef{BaseId::AoAttributeMap, true, "attribute_name", "AttributeName", "ATTR_NAME", "Attribute Name", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoAttributeMap, true, "name_mapping", "NameMap", "PARENT", "Name Map", DataType::DtLongLong, kIndex | kObligatory},
      // FILE
      BaseAttrDef{BaseId::AoFile, false, "ao_file_mimetype", "FileType", "FILE_TYPE", "File Type", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoFile, true, "ao_location", "FileLocation", "FILE_LOCATION", "File Location", DataType::DtString, kAutoGenerated},
      BaseAttrDef{BaseId::AoFile, false, "ao_size", "FileSize", "FILE_SIZE", "File Size", DataType::DtLongLong, kAutoGenerated},
      BaseAttrDef{BaseId::AoFile, false, "ao_original_filename", "FileOrig", "FILE_ORIG", "Original File Name", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoFile, false, "ao_original_filedate", "DateOrig", "DATE_ORIG", "Original File Date", DataType::DtDate, 0},
      BaseAttrDef{BaseId::AoFile, false, "ao_hash_algorithm", "HashType", "HASHTYPE", "Type of Hash Value", DataType::DtString, kAutoGenerated},
      BaseAttrDef{BaseId::AoFile, false, "ao_hash_value", "HashValue", "HASHVAL", "Hash Value", DataType::DtString, kAutoGenerated},
      BaseAttrDef{BaseId::AoFile, false, "ao_file_parent", "Parent", "PARENT", "Parent", DataType::DtLongLong, kIndex | kObligatory},
      // QUANTITY
      BaseAttrDef{BaseId::AoQuantity, false, "default_mq_name", "MqName", "MQ_NAME", "Default MQ Name", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoQuantity, false, "default_datatype", "DataType", "DATA_TYPE", "Data Type", DataType::DtEnum, kObligatory},
      BaseAttrDef{BaseId::AoQuantity, false, "default_rank", "Rank", "RANK", "Rank", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoQuantity, false, "default_type_size", "TypeSize", "TYPE_SIZE", "Size", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoQuantity, false, "default_unit", "Unit", "UNIT", "Unit", DataType::DtLongLong, 0},
      BaseAttrDef{BaseId::AoQuantity, false, "predecessor", "Father", "FATHER", "Father Quantity", DataType::DtLongLong,0},
      // UNIT
      BaseAttrDef{BaseId::AoUnit, true, "factor", "Factor", "FACTOR", "Factor", DataType::DtDouble, kObligatory},
      BaseAttrDef{BaseId::AoUnit, true, "offset", "Offset", "OFFSET_X", "Offset", DataType::DtDouble, kObligatory},
      BaseAttrDef{BaseId::AoUnit, true, "phys_dimension", "PhysDim", "PHYS_DIM", "Physical Dimension", DataType::DtLongLong, 0},
      // PHYS DIM
      BaseAttrDef{BaseId::AoPhysicalDimension, true, "length_exp", "LengthExp", "LENGTH_EXP", "Length Exponent", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, true, "mass_exp", "MassExp", "MASS_EXP", "Mass Exponent", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, true, "time_exp", "TimeExp", "TIME_EXP", "Time Exponent", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, true, "current_exp", "CurrExp", "CURR_EXP", "Current Exponent", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, true, "temperature_exp", "TempExp", "TEMP_EXP", "Temperature Exponent", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, true, "molar_amount_exp", "MolarExp", "MOL_EXP", "Molar Exponent", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, true, "luminous_intensity_exp", "LumExp", "LUM_EXP", "Luminous Exponent", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, false, "length_exp_den", "LengthExpDen", "LENGTH_EXP_DEN", "Length Exponent Denominator", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, false, "mass_exp_den", "MassExpDen", "MASS_EXP_DEN", "Mass Exponent Denominator", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, false, "time_exp_den", "TimeExpDen", "TIME_EXP_DEN", "Time Exponent Denominator", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, false, "current_exp_den", "CurrExpDen", "CURR_EXP_DEN", "Current Exponent Denominator", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, false, "temperature_exp_den", "TempExpDen", "TEMP_EXP_DEN", "Temperature Exponent Denominator", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, false, "molar_amount_exp_den", "MolarExpDen", "MOL_EXP_DEN", "Molar Exponent Denominator", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoPhysicalDimension, false, "luminous_intensity_exp_den", "LumExpDen", "LUM_EXP_DEN", "Luminous Exponent Denominator", DataType::DtLong, 0},
      // MEAS
      BaseAttrDef{BaseId::AoMeasurement, false, "measurement_begin", "MeasStart", "M_START", "Measurement Start", DataType::DtDate, 0},
      BaseAttrDef{BaseId::AoMeasurement, false, "measurement_end", "MeasStop", "M_STOP", "Measurement Stop", DataType::DtDate, 0},
      BaseAttrDef{BaseId::AoMeasurement, false, "ao_value_accessed", "Accessed", "ACC", "Value Accessed", DataType::DtDate, kAutoGenerated},
      BaseAttrDef{BaseId::AoMeasurement, false, "ao_value_accessed_by", "AccessedBy", "ACC_BY", "Value Accessed By", DataType::DtString, kAutoGenerated},
      BaseAttrDef{BaseId::AoMeasurement, false, "ao_value_modified", "ValueModified", "MOD", "Value Modified", DataType::DtDate, kAutoGenerated},
      BaseAttrDef{BaseId::AoMeasurement, false, "ao_value_modified_by", "ValueModifiedBy", "MOD_BY", "Value Accessed By", DataType::DtString, kAutoGenerated},
      BaseAttrDef{BaseId::AoMeasurement, false, "ao_storage_type", "StorageType", "STORAGE", "Storage Type", DataType::DtEnum, kObligatory | kAutoGenerated},
      BaseAttrDef{BaseId::AoMeasurement, false, "ao_mea_size", "MeasSize", "M_SIZE", "Measurement Size", DataType::DtLongLong, kAutoGenerated},
      BaseAttrDef{BaseId::AoMeasurement, true, "test", "Test", "PARENT", "Test", DataType::DtLongLong, kIndex},
      // MQ
      BaseAttrDef{BaseId::AoMeasurementQuantity, true, "datatype", "DataType", "DATA_TYPE", "Data Type", DataType::DtEnum, kObligatory},
      BaseAttrDef{BaseId::AoMeasurementQuantity, false, "type_size", "TypeSize", "TYPE_SIZE", "Size of Value", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoMeasurementQuantity, false, "interpolation", "Interpolation", "INT_POL", "Type of Interpolation", DataType::DtEnum, kObligatory},
      BaseAttrDef{BaseId::AoMeasurementQuantity, false, "minimum", "MinValue", "MIN_VAL", "Minimum Value", DataType::DtDouble, 0},
      BaseAttrDef{BaseId::AoMeasurementQuantity, false, "maximum", "MaxValue", "MAX_VAL", "Maximum Value", DataType::DtDouble, 0},
      BaseAttrDef{BaseId::AoMeasurementQuantity, false, "average", "AVERAGE", "AVG_VAL", "Average Value", DataType::DtDouble, 0},
      BaseAttrDef{BaseId::AoMeasurementQuantity, false, "standard_deviation", "StdDevValue", "DEV_VAL", "Standard Deviation Value", DataType::DtDouble, 0},
      BaseAttrDef{BaseId::AoMeasurementQuantity, true, "quantity", "Quantity", "QUANTITY", "Quantity", DataType::DtLongLong, kIndex},
      BaseAttrDef{BaseId::AoMeasurementQuantity, true, "unit", "Unit", "UNIT", "Unit", DataType::DtLongLong, kIndex},
      BaseAttrDef{BaseId::AoMeasurementQuantity, false, "channel", "Channel", "CHANNEL", "Device Channel", DataType::DtLongLong, kIndex},
      BaseAttrDef{BaseId::AoMeasurementQuantity, true, "measurement", "Measurement", "MEAS", "Measurement", DataType::DtLongLong, kIndex},
      // SM
      BaseAttrDef{BaseId::AoSubMatrix, true, "number_of_rows", "NofRows", "NOF_ROWS", "Number of Rows", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoSubMatrix, true, "measurement", "Measurement", "MEAS", "Measurement", DataType::DtLongLong, 0},
      // LC
      BaseAttrDef{BaseId::AoLocalColumn, false, "global_flag", "GlobalFlag", "G_FLAG", "Global Flag", DataType::DtShort, 0},
      BaseAttrDef{BaseId::AoLocalColumn, true, "independent", "Independent", "IND", "Independent Value", DataType::DtShort, 0},
      BaseAttrDef{BaseId::AoLocalColumn, false, "minimum", "MinValue", "MIN_VAL", "Minimum Value", DataType::DtDouble, 0},
      BaseAttrDef{BaseId::AoLocalColumn, false, "maximum", "MaxValue", "MAX_VAL", "Maximum Value", DataType::DtDouble, 0},
      BaseAttrDef{BaseId::AoLocalColumn, true, "sequence_representation", "Sequence", "SEQ", "Sequence", DataType::DtEnum, kObligatory},
      BaseAttrDef{BaseId::AoLocalColumn, false, "raw_datatype", "DataType", "DATA_TYPE", "Data Type", DataType::DtEnum, kObligatory},
      BaseAttrDef{BaseId::AoLocalColumn, false, "values", "ValueBlob", "VAL_BLOB", "Value Array", DataType::DtBlob, 0},
      BaseAttrDef{BaseId::AoLocalColumn, true, "submatrix", "SubMatrix", "SUB_MATRIX", "Sub-Matrix", DataType::DtLongLong, kIndex | kObligatory},
      BaseAttrDef{BaseId::AoLocalColumn, true, "measurement_quantity", "MeasQuantity", "MQ", "Measurement Quantity", DataType::DtLongLong, kIndex | kObligatory},
      BaseAttrDef{BaseId::AoLocalColumn, false, "flags", "QualityFlag", "QF", "Quality Flags", DataType::DtBlob, 0},
      // EXT COMP
      BaseAttrDef{BaseId::AoExternalComponent, false, "ordinal_number", "Order", "ORDINAL", "Order", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoExternalComponent, true, "component_length", "CompSize", "COMP_SIZE", "Component Size", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "filename_url", "Filename", "FILENAME", "File Name", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "value_type", "ValueType", "VAL_TYPE", "Type of Value", DataType::DtEnum, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "start_offset", "StartOffset", "START_OFF", "Start Offset", DataType::DtLongLong, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "block_size", "BlockSize", "BLOCK_SIZE", "Block Size", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "valuesperblock", "ValuesPerBlock", "VAL_BLOCK", "Values/Block", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "value_offset", "ValueOffset", "VAL_OFF", "Value Offset", DataType::DtLong, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "flags_filename_url", "FlagFile", "FLAG_FILE", "Flags File", DataType::DtString, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "flags_start_offset", "FlagOffset", "FLAG_OFF", "Flags Offset", DataType::DtLongLong, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "ao_bit_count", "BitCount", "BIT_COUNT", "Bit Count", DataType::DtShort, 0},
      BaseAttrDef{BaseId::AoExternalComponent, false, "ao_bit_offset", "BitOffset", "BIT_OFF", "Bit  Offset", DataType::DtShort, 0},
      BaseAttrDef{BaseId::AoExternalComponent, true, "local_column", "LocalColumn", "LOC_COL", "Local Column", DataType::DtLongLong, kIndex | kObligatory},
      BaseAttrDef{BaseId::AoExternalComponent, false, "ao_values_file", "ValuesFile", "VALS_FILE", "Values File", DataType::DtLongLong, kIndex },
      BaseAttrDef{BaseId::AoExternalComponent, false, "ao_flags_file", "FlagsFile", "FLAGS_FILE", "Flags File", DataType::DtLongLong, kIndex },
      // SUB TEST
      BaseAttrDef{BaseId::AoSubTest, true, "parent_test", "Test", "TEST", "Test", DataType::DtLongLong, kIndex | kObligatory },
      // UUT PART
      BaseAttrDef{BaseId::AoUnitUnderTestPart, false, "parent_unit_under_test", "UnitUnderTest", "UUT", "Unit Under Test", DataType::DtLongLong, kIndex | kObligatory },
      BaseAttrDef{BaseId::AoUnitUnderTestPart, false, "parent_unit_under_test_part", "UnitUnderTestPart", "UUTP", "Unit Under Test Part", DataType::DtLongLong, kIndex | kObligatory },
      // TEST SEQ PART
      BaseAttrDef{BaseId::AoTestSequencePart, false, "parent_sequence", "TestSequence", "TEST_SEQ", "Test Sequence", DataType::DtLongLong, kIndex | kObligatory },
      BaseAttrDef{BaseId::AoTestSequencePart, false, "parent_sequence_part", "TestSequencePart", "TEST_SEQP", "Test Sequence Part", DataType::DtLongLong, kIndex | kObligatory },
      // TEST EQ
      BaseAttrDef{BaseId::AoTestEquipmentPart, false, "parent_equipment", "TestEquipment", "TEST_EQ", "Test Equipment", DataType::DtLongLong, kIndex | kObligatory },
      BaseAttrDef{BaseId::AoTestEquipmentPart, false, "parent_equipment_part", "TestEquipmentPart", "TEST_EQP", "Test Equipment Part", DataType::DtLongLong, kIndex | kObligatory },
      // TEST DEV
      BaseAttrDef{BaseId::AoTestDevice, false, "parent_equipment", "TestEquipment", "TEST_EQ", "Test Equipment", DataType::DtLongLong, kIndex | kObligatory },
      BaseAttrDef{BaseId::AoTestDevice, false, "parent_equipment_part", "TestEquipmentPart", "TEST_EQP", "Test Equipment Part", DataType::DtLongLong, kIndex | kObligatory },
      // USER
      BaseAttrDef{BaseId::AoUser, true, "password", "Password", "PASSWORD", "Password", DataType::DtString, 0 },
      BaseAttrDef{BaseId::AoUser, true, "ao_disables", "Disabled", "DISABLED", "Disabled", DataType::DtShort, 0 },
      // USER GROUP
      BaseAttrDef{BaseId::AoUserGroup, true, "super_user_flag", "Password", "PASSWORD", "Password", DataType::DtShort, 0 },
      // ANY
      BaseAttrDef{BaseId::AoAny, false, "parent", "Parent", "PARENT", "Parent", DataType::DtLongLong, kIndex | kObligatory },
      // LOG
      BaseAttrDef{BaseId::AoLog, true, "date", "LogTime", "LOG_TIME", "Log Time", DataType::DtDate, 0 },
      BaseAttrDef{BaseId::AoLog, false, "parent", "Parent", "PARENT", "Parent Log", DataType::DtLongLong, kIndex },
      // PARAMETER
      BaseAttrDef{BaseId::AoParameter, true, "parameter_datatype", "DataType", "DATA_TYPE", "Data Type", DataType::DtEnum, kObligatory },
      BaseAttrDef{BaseId::AoParameter, true, "pvalue", "ParameterValue", "PAR_VAL", "Parameter Value", DataType::DtString, 0 },
      BaseAttrDef{BaseId::AoParameter, false, "unit", "Unit", "UNIT", "Unit", DataType::DtLongLong, 0 },
      BaseAttrDef{BaseId::AoParameter, true, "parameter_set", "ParSet", "PAR_SET", "Parameter Set", DataType::DtLongLong, kIndex | kObligatory },
  };
} // end namespace

namespace ods {

BaseAttribute::BaseAttribute(const std::string &base_name) {
  const auto itr = std::ranges::find_if(kBaseAttrList, [&] (const BaseAttrDef& attr) {
    return IEquals(attr.BaseName.data(), base_name);
  });
  BaseName(base_name);
  if (itr != kBaseAttrList.cend()) {
    Mandatory(itr->Mandatory);
    ApplicationName(itr->AppName.data());
    BaseName(itr->BaseName.data());
    DatabaseName(itr->DbName.data());
    DisplayName(itr->DisplayName.data());
    DataType(itr->Type);
    Flags(itr->Flags);
  } else {
    BaseName(base_name);
  }

  if (BaseName() == "default_datatype" || BaseName() == "datatype"
         || BaseName() == "parameter_datatype" || BaseName() == "raw_datatype") {
    EnumName("datatype_enum");
  } else if (BaseName() == "ao_storage_type") {
    EnumName("ao_storagetype_enum");
  } else if (BaseName() == "ao_storage_type") {
    EnumName("interpolation_enum");
  } else if (BaseName() == "sequence_representation") {
    EnumName("seq_rep_enum");
  } else if (BaseName() == "value_type") {
    EnumName("typespec_enum");
  }
}

std::vector<BaseAttribute> GetBaseAttributeList(BaseId base_id) {

  std::vector<BaseAttribute> attr_list;

  for (const auto& base1 : kBaseAttrList) {
    if (base1.Mandatory && base1.BelongsTo == BaseId::AoNotDefined) {
      attr_list.emplace_back(BaseAttribute(base1.BaseName.data()));
    }
  }

  for (const auto& base2 : kBaseAttrList) {
    if (base2.Mandatory && base2.BelongsTo == base_id && base_id != BaseId::AoNotDefined) {
      attr_list.emplace_back(BaseAttribute(base2.BaseName.data()));
    }
  }

  for (const auto& base3 : kBaseAttrList) {
    if (!base3.Mandatory && base3.BelongsTo == base_id && base_id != BaseId::AoNotDefined) {
      attr_list.emplace_back(BaseAttribute(base3.BaseName.data()));
    }
  }

  for (const auto& base4 : kBaseAttrList) {
    if (!base4.Mandatory && base4.BelongsTo == BaseId::AoNotDefined) {
      attr_list.emplace_back(BaseAttribute(base4.BaseName.data()));
    }
  }
  return attr_list;
}

std::vector<std::string> GetParentBaseName(BaseId base_id) {
  std::vector<std::string> base_list;
  switch (base_id) {
    case BaseId::AoAttributeMap:
      base_list.emplace_back("name_mapping");
      break;

    case BaseId::AoMeasurement:
      base_list.emplace_back("test");
      break;

    case BaseId::AoMeasurementQuantity:
    case BaseId::AoSubMatrix:
      base_list.emplace_back("measurement");
      break;

    case BaseId::AoLocalColumn:
      base_list.emplace_back("submatrix");
      break;

    case BaseId::AoExternalComponent:
      base_list.emplace_back("local_column");
      break;

    case BaseId::AoSubTest:
      base_list.emplace_back("parent");
      base_list.emplace_back("parent_test");
      break;

    case BaseId::AoUnitUnderTestPart:
      base_list.emplace_back("parent");
      base_list.emplace_back("parent_unit_under_test");
      base_list.emplace_back("parent_unit_under_test_part");
      break;

    case BaseId::AoTestSequencePart:
      base_list.emplace_back("parent");
      base_list.emplace_back("parent_sequence");
      base_list.emplace_back("parent_sequence_part");
      break;

    case BaseId::AoTestEquipmentPart:
    case BaseId::AoTestDevice:
      base_list.emplace_back("parent");
      base_list.emplace_back("parent_equipment");
      base_list.emplace_back("parent_equipment_part");
      break;

    case BaseId::AoAny:
    case BaseId::AoLog:
      base_list.emplace_back("parent");
      break;

    case BaseId::AoParameter:
      base_list.emplace_back("parameter_set");
      break;

    case BaseId::AoFile:
      base_list.emplace_back("parent");
      base_list.emplace_back("parent_test");
      base_list.emplace_back("ao_file_parent");
      break;

    default:
      break;
  }
  return base_list;
}
} // end namespace
