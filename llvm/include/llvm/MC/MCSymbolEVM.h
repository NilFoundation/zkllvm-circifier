//===- MCSymbolEVM.h -------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_MC_MCSYMBOLEVM_H
#define LLVM_MC_MCSYMBOLEVM_H

#include "llvm/BinaryFormat/EVM.h"
#include "llvm/MC/MCSymbol.h"

namespace llvm {

class GlobalValue;

class MCSymbolEVM : public MCSymbol {

public:

  enum SymbolKind {
    KindNone        = 0,
    KindFunction    = 1,
    KindGlobalValue = 2,
  };

  MCSymbolEVM(const StringMapEntry<bool> *Name, bool isTemporary)
      : MCSymbol(SymbolKindEVM, Name, isTemporary) {}

  static bool classof(const MCSymbol *S) { return S->isEVM(); }

  void setKind(SymbolKind T) {
    Kind = T;
  }
  SymbolKind getKind() const {
    return Kind;
  }
  bool isFunction() const {
    return getKind() == KindFunction;
  }
  bool isGlobalValue() const {
    return getKind() == KindGlobalValue;
  }
  void setHidden(bool v) {
    IsHidden = v;
  }
  bool isHidden() const {
    return IsHidden;
  }
  void setSize(unsigned S) {
    Size = S;
  }
  unsigned getSize() const {
    return Size;
  }
  EVM::EVMSignature* getSignature() {
    return &Signature;
  }
  const EVM::EVMSignature* getSignature() const {
    return &Signature;
  }
private:
  EVM::EVMSignature Signature;
  SymbolKind Kind{KindNone};
  unsigned Size{0};
  // All symbols are hidden by default.
  bool IsHidden{true};
};

} // end namespace llvm

#endif // LLVM_MC_MCSYMBOLEVM_H
