//===- MCSectionEVM.h - EVM Machine Code Sections ---------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the MCSectionEVM class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCSECTIONEVM_H
#define LLVM_MC_MCSECTIONEVM_H

#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSymbolEVM.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

class MCSymbol;
class Function;

/// This represents a section of EVM.
class MCSectionEVM final : public MCSection {

  struct FunctionInfo {
    llvm::Function *F;
    unsigned Offset;
  };

public:
  friend class MCContext;
  MCSectionEVM(SectionKind K, MCSymbol *Begin)
      : MCSection(SV_EVM, ".evm", K, Begin) {}

  void printSwitchToSection(const MCAsmInfo &MAI, const Triple &T,
                            raw_ostream &OS,
                            const MCExpr *Subsection) const override {}
  bool useCodeAlign() const override {
    return false;
  }
  bool isVirtualSection() const override {
    return false;
  }
  void addFunction(Function *F, unsigned O) {
    Functions.push_back({F, O});
  }
  const SmallVector<FunctionInfo>& functions() {
    return Functions;
  }
  void addFixupOffset(unsigned O) {
    FixupOffsets.push_back(O);
  }
  const SmallVector<unsigned>& fixupOffsets() {
    return FixupOffsets;
  }

private:
  SmallVector<FunctionInfo> Functions;
  // Fixup offset is an offset of PUSH's immediate value, that represents offset
  // for a jump instruction.
  SmallVector<unsigned> FixupOffsets;
};

} // end namespace llvm

#endif
