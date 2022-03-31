/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include "ods/ienum.h"

namespace ods {

enum class BaseId : int {
  AoAny                 = 0,
  AoEnvironment         = 1,
  AoSubTest             = 2,
  AoMeasurement         = 3,
  AoMeasurementQuantity = 4,
  AoQuantity            = 11,
  AoQuantityGroup       = 12,
  AoUnit                = 13,
  AoUnitGroup           = 14,
  AoPhysicalDimension   = 15,
  AoUnitUnderTest       = 21,
  AoUnitUnderTestPart   = 22,
  AoTestEquipment       = 23,
  AoTestEquipmentPart   = 24,
  AoTestSequence        = 25,
  AoTestSequencePart    = 26,
  AoUser                = 34,
  AoUserGroup           = 35,
  AoTest                = 36,
  AoTestDevice          = 37,
  AoSubMatrix           = 38,
  AoLocalColumn         = 39,
  AoExternalComponent   = 40,
  AoLog                 = 43,
  AoParameter           = 44,
  AoParameterSet        = 45,
  AoNameMap             = 46,
  AoAttributeMap        = 47,
  AoFile                = 48,
  AoMimetypeMap                = 49,
  AoNotDefined          = 255
};

BaseId TextToBaseId(const std::string& text);
std::string BaseIdToText(BaseId base);
std::string BaseIdToUserText(BaseId base);

enum class DataType : int {
  DtUnknown     = 0,
  DtString      = 1,
  DtShort       = 2,
  DtFloat       = 3,
  DtBoolean     = 4,
  DtByte        = 5,
  DtLong        = 6,
  DtDouble      = 7,
  DtLongLong    = 8,
  DtId          = 9,
  DtDate        = 10,
  DtByteString  = 11,
  DtBlob        = 12,
  DtComplex     = 13,
  DtDComplex    = 14,

  DsString      = 15,
  DsShort       = 16,
  DsFloat       = 17,
  DsBoolean     = 18,
  DsByte        = 19,
  DsLong        = 20,
  DsDouble      = 21,
  DsLongLong    = 22,
  DsComplex     = 23,
  DsDComplex    = 24,
  DsId          = 25,
  DsDate        = 26,
  DsByteString  = 27,

  DtExternalRef = 28,
  DtEnum        = 30,
  DsEnum        = 31
};

DataType TextToDataType(const std::string& text);
std::string DataTypeToText(DataType type);
std::string DataTypeToUserText(DataType type);

enum class SequenceRepresentation : int {
  Explicit = 0,
  ImplicitConstant = 1,
  ImplicitLinear = 2,
  ImplicitSaw = 3,
  RawLinear = 4,
  RawPolynomial = 5,
  Formula = 6,
  External = 7,
  RawLinearExternal = 8,
  RawPolynomialExternal = 9,
  RawLinearCalibrated = 10,
  RawCalibratedExternal = 11
};

constexpr uint16_t kUnique = 0x01;
constexpr uint16_t kObligatory = 0x02;
constexpr uint16_t kAutoGenerated = 0x04;
constexpr uint16_t kCaseSensitive = 0x08;
constexpr uint16_t kIndex = 0x80;

IEnum CreateDefaultEnum(const std::string& enum_name);


} // end namespace