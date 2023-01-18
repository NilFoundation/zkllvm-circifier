#ifndef LLVM_ZK_FIELD_KINDS_H
#define LLVM_ZK_FIELD_KINDS_H

#include "llvm/Support/ErrorHandling.h"

namespace llvm {
enum GaloisFieldKind : unsigned {
#define GALOIS_FIELD_TYPE(Name, EnumId, SingletonId, FrontendId)  \
  EnumId,
#include "llvm/IR/GaloisFieldTypes.def"
};

inline unsigned GetNumberBits(GaloisFieldKind Kind) {
  // TODO(maksenov): get rid of explicit numbers here
  switch (Kind) {
  case GALOIS_FIELD_BLS12381_BASE:
    return 381;
  case GALOIS_FIELD_BLS12381_SCALAR:
    return 255;
  case GALOIS_FIELD_PALLAS_BASE:
    return 255;
  case GALOIS_FIELD_PALLAS_SCALAR:
    return 255;
  case GALOIS_FIELD_VESTA_BASE:
    return 255;
  case GALOIS_FIELD_VESTA_SCALAR:
    return 255;
  case GALOIS_FIELD_CURVE25519_BASE:
    return 255;
  case GALOIS_FIELD_CURVE25519_SCALAR:
    return 253;
  }
  llvm_unreachable("Unspecified field kind");
}
} // namespace llvm

#endif // LLVM_ZK_FIELD_KINDS_H
