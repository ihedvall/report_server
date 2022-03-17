/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string_view>
#include <array>
#include "ods/odsdef.h"
#include "util/stringutil.h"

using namespace util::string;
namespace {

struct BaseIdDef {
  const int BaseId;
  const std::string_view BaseName1;
  const std::string_view BaseName2;
};

constexpr std::array<BaseIdDef,31> kBaseIdList = {
    BaseIdDef{0, "AoAny", "Any"},
    BaseIdDef{1, "AoEnvironment", "Environment"},
    BaseIdDef{2, "AoSubTest", "SubTest"},
    BaseIdDef{3, "AoMeasurement", "Measurement"},
    BaseIdDef{4, "AoMeasurementQuantity", "MeasurementQuantity"},
    BaseIdDef{11, "AoQuantity", "Quantity"},
    BaseIdDef{12, "AoQuantityGroup", "QuantityGroup"},
    BaseIdDef{13, "AoUnit", "Unit"},
    BaseIdDef{14, "AoUnitGroup", "UnitGroup"},
    BaseIdDef{15, "AoPhysicalDimension", "PhysicalDimension"},
    BaseIdDef{21, "AoUnitUnderTest", "UnitUnderTest"},
    BaseIdDef{22, "AoUnitUnderTestPart", "UnitUnderTestPart"},
    BaseIdDef{23, "AoTestEquipment", "TestEquipment"},
    BaseIdDef{24, "AoTestEquipmentPart", "TestEquipmentPart"},
    BaseIdDef{25, "AoTestSequence", "TestSequence"},
    BaseIdDef{26, "AoTestSequencePart", "TestSequencePart"},
    BaseIdDef{34, "AoUser", "User"},
    BaseIdDef{35, "AoUserGroup", "UserGroup"},
    BaseIdDef{36, "AoTest", "Test"},
    BaseIdDef{37, "AoTestDevice", "TestDevice"},
    BaseIdDef{38, "AoSubMatrix", "SubMatrix"},
    BaseIdDef{39, "AoLocalColumn", "LocalColumn"},
    BaseIdDef{40, "AoExternalComponent", "ExternalComponent"},
    BaseIdDef{43, "AoLog", "Log"},
    BaseIdDef{44, "AoParameter", "Parameter"},
    BaseIdDef{45, "AoParameterSet", "ParameterSet"},
    BaseIdDef{46, "AoNameMap", "NameMap"},
    BaseIdDef{47, "AoAttributeMap", "AttributeMap"},
    BaseIdDef{48, "AoFile", "File"},
    BaseIdDef{49, "AoView", "View"},
    BaseIdDef{255, "AoNotDefined", "NotDefined"},
};

struct DataTypeDef {
  const int DataType;
  const std::string_view TypeName1;
  const std::string_view TypeName2;
  const std::string_view TypeName3;
};

constexpr std::array<DataTypeDef,31> kDataTypeList = {
    DataTypeDef{0, "DtUnknown", "DT_UNKNOWN", "Unknown"},
    DataTypeDef{1, "DtString", "DT_STRING", "String"},
    DataTypeDef{2, "DtShort", "DT_SHORT", "Short"},
    DataTypeDef{3, "DtFloat", "DT_FLOAT", "Float"},
    DataTypeDef{4, "DtBoolean", "DT_BOOLEAN", "Boolean"},
    DataTypeDef{5, "DtByte", "DT_BYTE", "Byte"},
    DataTypeDef{6, "DtLong", "DT_LONG", "Long"},
    DataTypeDef{7, "DtDouble", "DT_DOUBLE", "Double"},
    DataTypeDef{8, "DtLongLong", "DT_LONGLONG", "LongLong"},
    DataTypeDef{9, "DtId", "DT_ID", "Id"},
    DataTypeDef{10, "DtDate", "DT_DATE", "Date"},
    DataTypeDef{11, "DtByteString", "DT_BYTESTRING", "ByteString"},
    DataTypeDef{12, "DtBlob", "DT_BLOB", "Blob"},
    DataTypeDef{13, "DtComplex", "DT_COMPLEX", "Complex"},
    DataTypeDef{14, "DtDComplex", "DT_DCOMPLEX", "DComplex"},

    DataTypeDef{15, "DsString", "DS_STRING", "StringSeq"},
    DataTypeDef{16, "DsShort", "DS_SHORT", "ShortSeq"},
    DataTypeDef{17, "DsFloat", "DS_FLOAT", "FloatSeq"},
    DataTypeDef{18, "DsBoolean", "DS_BOOLEAN", "BooleanSeq"},
    DataTypeDef{19, "DsByte", "DS_BYTE", "ByteSeq"},
    DataTypeDef{20, "DsLong", "DS_LONG", "LongSeq"},
    DataTypeDef{21, "DsDouble", "DS_DOUBLE", "DoubleSeq"},
    DataTypeDef{22, "DsLongLong", "DS_LONGLONG", "LongLongSeq"},
    DataTypeDef{23, "DsComplex", "DS_COMPLEX", "ComplexSeq"},
    DataTypeDef{24, "DsDComplex", "DS_DCOMPLEX", "DComplexSeq"},
    DataTypeDef{25, "DsId", "DS_ID", "IdSeq"},
    DataTypeDef{26, "DsDate", "DS_DATE", "DateSeq"},
    DataTypeDef{27, "DsByteString", "DS_BYTESTRING", "ByteStringSeq"},

    DataTypeDef{28, "DtExternalRef", "DT_EXTERNALREF", "ExternalRef"},
    DataTypeDef{30, "DtEnum", "DT_ENUM", "Enum"},
    DataTypeDef{31, "DsEnum", "DS_ENUM", "EnumSeq"},
};

}
namespace ods {

BaseId TextToBaseId(const std::string &text) {
  int temp = -1;
  try {
    temp = std::stoi(text);
  } catch(const std::exception&) {
    // Normal if not an integer
  }

  for (const auto base : kBaseIdList) {
    if (temp == base.BaseId ||
       IEquals(text,base.BaseName1.data()) ||
       IEquals(text,base.BaseName2.data())) {
      return static_cast<BaseId>(base.BaseId);
    }
  }
  return BaseId::AoNotDefined;
}

std::string BaseIdToText(BaseId base) {
  const int temp = static_cast<int> (base);

  for (const auto bid : kBaseIdList) {
    if (temp == bid.BaseId) {
      return bid.BaseName1.data();
    }
  }
  return "AoNotDefined";
}
DataType TextToDataType(const std::string &text) {
  int temp = -1;
  try {
    temp = std::stoi(text);
  } catch(const std::exception&) {
    // Normal if not an integer
  }

  for (const auto type : kDataTypeList) {
    if (temp == type.DataType ||
        IEquals(text,type.TypeName1.data()) ||
        IEquals(text,type.TypeName2.data()) ||
        IEquals(text,type.TypeName3.data())) {
      return static_cast<DataType>(type.DataType);
    }
  }
  return DataType::DtUnknown;
}

std::string DataTypeToText(DataType type) {
  const int temp = static_cast<int> (type);

  for (const auto tid : kDataTypeList) {
    if (temp == tid.DataType) {
      return tid.TypeName1.data();
    }
  }
  return "DtUnknown";
}

std::string DataTypeToUserText(DataType type) {
  const int temp = static_cast<int> (type);

  for (const auto tid : kDataTypeList) {
    if (temp == tid.DataType) {
      return tid.TypeName3.data();
    }
  }
  return "Unknown";
}
}

