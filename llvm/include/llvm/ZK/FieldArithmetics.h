#ifndef LLVM_ZK_FIELD_ARITHMETICS_H
#define LLVM_ZK_FIELD_ARITHMETICS_H

#include "llvm/ADT/FieldElem.h"

namespace llvm {

enum FieldOperation {
  F_Add,
  F_Sub,
  F_Mul,
  F_Div,
};

FieldElem FieldBinOp(FieldOperation Op, const FieldElem &LHS,
                     const FieldElem &RHS);

bool FieldElemFromStr(GaloisFieldKind Kind, StringRef Str, FieldElem &Result);
}


#endif  // LLVM_ZK_FIELD_ARITHMETICS_H
