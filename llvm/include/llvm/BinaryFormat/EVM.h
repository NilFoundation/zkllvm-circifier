//===- EVM.h - EVM object file format -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines manifest constants for the EVM object file format.
// See: https://github.com/WebAssembly/design/blob/master/BinaryEncoding.md
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_BINARYFORMAT_EVM_H
#define LLVM_BINARYFORMAT_EVM_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {
namespace EVM {

// Object file magic string.
const char EVMMagic[] = {'\0', 'a', 's', 'm'};
// EVM binary format version
const uint32_t EVMVersion = 0x1;
// EVM linking metadata version
const uint32_t EVMMetadataVersion = 0x2;
// EVM uses a 64k page size
const uint32_t EVMPageSize = 65536;

struct EVMObjectHeader {
  StringRef Magic;
  uint32_t Version;
};

struct EVMDylinkInfo {
  uint32_t MemorySize; // Memory size in bytes
  uint32_t MemoryAlignment;  // P2 alignment of memory
  uint32_t TableSize;  // Table size in elements
  uint32_t TableAlignment;  // P2 alignment of table
  std::vector<StringRef> Needed; // Shared library depenedencies
};

struct EVMProducerInfo {
  std::vector<std::pair<std::string, std::string>> Languages;
  std::vector<std::pair<std::string, std::string>> Tools;
  std::vector<std::pair<std::string, std::string>> SDKs;
};

struct EVMFeatureEntry {
  uint8_t Prefix;
  std::string Name;
};

struct EVMExport {
  StringRef Name;
  uint8_t Kind;
  uint32_t Index;
};

struct EVMLimits {
  uint8_t Flags;
  uint32_t Initial;
  uint32_t Maximum;
};

struct EVMTable {
  uint8_t ElemType;
  EVMLimits Limits;
};

struct EVMInitExpr {
  uint8_t Opcode;
  union {
    int32_t Int32;
    int64_t Int64;
    int32_t Float32;
    int64_t Float64;
    uint32_t Global;
  } Value;
};

struct EVMGlobalType {
  uint8_t Type;
  bool Mutable;
};

struct EVMGlobal {
  uint32_t Index;
  EVMGlobalType Type;
  EVMInitExpr InitExpr;
  StringRef SymbolName; // from the "linking" section
};

struct EVMEventType {
  // Kind of event. Currently only EVM_EVENT_ATTRIBUTE_EXCEPTION is possible.
  uint32_t Attribute;
  uint32_t SigIndex;
};

struct EVMEvent {
  uint32_t Index;
  EVMEventType Type;
  StringRef SymbolName; // from the "linking" section
};

struct EVMImport {
  StringRef Module;
  StringRef Field;
  uint8_t Kind;
  union {
    uint32_t SigIndex;
    EVMGlobalType Global;
    EVMTable Table;
    EVMLimits Memory;
    EVMEventType Event;
  };
};

struct EVMLocalDecl {
  uint8_t Type;
  uint32_t Count;
};

struct EVMFunction {
  uint32_t Index;
  std::vector<EVMLocalDecl> Locals;
  ArrayRef<uint8_t> Body;
  uint32_t CodeSectionOffset;
  uint32_t Size;
  uint32_t CodeOffset;  // start of Locals and Body
  StringRef SymbolName; // from the "linking" section
  StringRef DebugName;  // from the "name" section
  uint32_t Comdat;      // from the "comdat info" section
};

struct EVMDataSegment {
  uint32_t InitFlags;
  uint32_t MemoryIndex; // present if InitFlags & EVM_SEGMENT_HAS_MEMINDEX
  EVMInitExpr Offset; // present if InitFlags & EVM_SEGMENT_IS_PASSIVE == 0
  ArrayRef<uint8_t> Content;
  StringRef Name; // from the "segment info" section
  uint32_t Alignment;
  uint32_t LinkerFlags;
  uint32_t Comdat; // from the "comdat info" section
};

struct EVMElemSegment {
  uint32_t TableIndex;
  EVMInitExpr Offset;
  std::vector<uint32_t> Functions;
};

// Represents the location of a EVM data symbol within a EVMDataSegment, as
// the index of the segment, and the offset and size within the segment.
struct EVMDataReference {
  uint32_t Segment;
  uint32_t Offset;
  uint32_t Size;
};

struct EVMRelocation {
  uint8_t Type;    // The type of the relocation.
  uint32_t Index;  // Index into either symbol or type index space.
  uint64_t Offset; // Offset from the start of the section.
  int64_t Addend;  // A value to add to the symbol.
};

struct EVMInitFunc {
  uint32_t Priority;
  uint32_t Symbol;
};

struct EVMSymbolInfo {
  StringRef Name;
  uint8_t Kind;
  uint32_t Flags;
  StringRef ImportModule; // For undefined symbols the module of the import
  StringRef ImportName;   // For undefined symbols the name of the import
  union {
    // For function or global symbols, the index in function or global index
    // space.
    uint32_t ElementIndex;
    // For a data symbols, the address of the data relative to segment.
    EVMDataReference DataRef;
  };
};

struct EVMFunctionName {
  uint32_t Index;
  StringRef Name;
};

struct EVMLinkingData {
  uint32_t Version;
  std::vector<EVMInitFunc> InitFunctions;
  std::vector<StringRef> Comdats;
  std::vector<EVMSymbolInfo> SymbolTable;
};

enum : unsigned {
  EVM_SEC_CUSTOM = 0,     // Custom / User-defined section
  EVM_SEC_TYPE = 1,       // Function signature declarations
  EVM_SEC_IMPORT = 2,     // Import declarations
  EVM_SEC_FUNCTION = 3,   // Function declarations
  EVM_SEC_TABLE = 4,      // Indirect function table and other tables
  EVM_SEC_MEMORY = 5,     // Memory attributes
  EVM_SEC_GLOBAL = 6,     // Global declarations
  EVM_SEC_EXPORT = 7,     // Exports
  EVM_SEC_START = 8,      // Start function declaration
  EVM_SEC_ELEM = 9,       // Elements section
  EVM_SEC_CODE = 10,      // Function bodies (code)
  EVM_SEC_DATA = 11,      // Data segments
  EVM_SEC_DATACOUNT = 12, // Data segment count
  EVM_SEC_EVENT = 13      // Event declarations
};

// Type immediate encodings used in various contexts.
enum : unsigned {
  EVM_TYPE_I32 = 0x7F,
  EVM_TYPE_I64 = 0x7E,
  EVM_TYPE_I128 = 0x7B,
  EVM_TYPE_I256 = 0x7A,
  EVM_TYPE_FUNCREF = 0x70,
  EVM_TYPE_EXCEPT_REF = 0x68,
  EVM_TYPE_FUNC = 0x60,
  EVM_TYPE_NORESULT = 0x40, // for blocks with no result values
};

// Kinds of event attributes.
enum EVMEventAttribute : unsigned {
  EVM_EVENT_ATTRIBUTE_EXCEPTION = 0x0,
};

// Subset of types that a value can have
enum class ValType {
  INVALID = 0,
  I32 = EVM_TYPE_I32,
  I64 = EVM_TYPE_I64,
  I128 = EVM_TYPE_I128,
  I256 = EVM_TYPE_I256,
};

inline const char* ValTypeToString(ValType Type) {
  switch (Type) {
  case ValType::I32: return "int32";
  case ValType::I64: return "int64";
  case ValType::I128: return "int128";
  case ValType::I256: return "int256";
  default:
    llvm_unreachable("Invalid type!");
  }
}

inline ValType parseType(StringRef Str) {
  if (Str == "int32")
    return ValType::I32;
  if (Str == "int64")
    return ValType::I64;
  if (Str == "int128")
    return ValType::I128;
  if (Str == "int256")
    return ValType::I256;
  return ValType::INVALID;
}

struct EVMSignature {
  SmallVector<ValType, 2> Returns;
  SmallVector<ValType, 4> Params;
  // Support empty and tombstone instances, needed by DenseMap.
  enum { Plain, Empty, Tombstone } State = Plain;

  EVMSignature(decltype(Returns) &&InReturns, decltype(Params) &&InParams)
      : Returns(std::forward<decltype(Returns)>(InReturns)),
        Params(std::forward<decltype(Params)>(InParams)) {}
  EVMSignature() = default;
};

// Useful comparison operators
inline bool operator==(const EVMSignature &LHS, const EVMSignature &RHS) {
  return LHS.State == RHS.State && LHS.Returns == RHS.Returns &&
         LHS.Params == RHS.Params;
}

inline bool operator!=(const EVMSignature &LHS, const EVMSignature &RHS) {
  return !(LHS == RHS);
}

inline bool operator==(const EVMGlobalType &LHS, const EVMGlobalType &RHS) {
  return LHS.Type == RHS.Type && LHS.Mutable == RHS.Mutable;
}

inline bool operator!=(const EVMGlobalType &LHS, const EVMGlobalType &RHS) {
  return !(LHS == RHS);
}


} // end namespace EVM
} // end namespace llvm

#endif
