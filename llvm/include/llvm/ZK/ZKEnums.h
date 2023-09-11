#ifndef LLVM_ZK_ZK_ENUMS_H
#define LLVM_ZK_ZK_ENUMS_H

#include "llvm/Support/ErrorHandling.h"

namespace llvm {
enum GaloisFieldKind : unsigned {
#define GALOIS_FIELD_TYPE(Name, EnumId, SingletonId, FrontendId)  \
  EnumId,
#include "llvm/IR/GaloisFieldTypes.def"
};

enum EllipticCurveKind : unsigned {
#define ELLIPTIC_CURVE_TYPE(Name, EnumId, SingletonId, FrontendId)  \
  EnumId,
#include "llvm/IR/EllipticCurveTypes.def"
};

inline llvm::GaloisFieldKind GetBaseFieldKind(EllipticCurveKind CurveKind) {
  switch (CurveKind) {
#define CURVE_BASE_FIELD_MAPPING(CurveKind, CurveFrontendId, FieldKind, FieldFrontendId) \
  case CurveKind: return FieldKind;
#include "llvm/IR/EllipticCurveTypes.def"
  }
  llvm_unreachable("Base field type for curve is not defined");
}

unsigned GetNumberBits(GaloisFieldKind Kind);
unsigned GetNumberBits(EllipticCurveKind Kind);
} // namespace llvm

#endif // LLVM_ZK_ZK_ENUMS_H
