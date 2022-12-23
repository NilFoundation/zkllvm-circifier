//===-- llvm/ADT/FieldElem.h ----- Galois field element --------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the FieldElem class, which is a simple class that
/// represents an galois field element.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADT_FIELDELEM_H
#define LLVM_ADT_FIELDELEM_H

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ZK/FieldKinds.h"

namespace llvm {

/// Galois field element.
class [[nodiscard]] FieldElem : public APInt {
  GaloisFieldKind Kind;

public:
  FieldElem() : APInt(), Kind(static_cast<GaloisFieldKind>(0)) {}
  FieldElem(GaloisFieldKind k, APInt v) : APInt(v), Kind(k) {
    if (v.isSingleWord())
      APInt::operator=(APInt(GetNumberBits(Kind), v.getZExtValue()));
  }

  GaloisFieldKind getKind() const { return Kind; }

  FieldElem &operator+=(const FieldElem &other);

  uint64_t *getData() const {
    assert(!isSingleWord());
    return U.pVal;
  }
};

inline FieldElem operator+(FieldElem a, const FieldElem &b) {
  a += b;
  return a;
}

/// Provide DenseMapInfo for APSInt, using the DenseMapInfo for APInt.
template <> struct DenseMapInfo<FieldElem, void> {
  static inline FieldElem getEmptyKey() {
    return FieldElem(static_cast<GaloisFieldKind>(0), DenseMapInfo<APInt, void>::getEmptyKey());
  }

  static inline FieldElem getTombstoneKey() {
    return FieldElem(static_cast<GaloisFieldKind>(0), DenseMapInfo<APInt, void>::getTombstoneKey());
  }

  static unsigned getHashValue(const FieldElem &Key) {
    return hash_combine(Key.getKind(), DenseMapInfo<APInt, void>::getHashValue(Key));
  }

  static bool isEqual(const FieldElem &LHS, const FieldElem &RHS) {
    return LHS.getBitWidth() == RHS.getBitWidth() &&
           LHS.getKind() == RHS.getKind() && LHS == RHS;
  }
};
} // namespace llvm

#endif  // LLVM_ADT_FIELDELEM_H
