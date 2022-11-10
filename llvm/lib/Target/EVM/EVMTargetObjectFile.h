//===-- EVMTargetObjectFile.h - EVM Object Info -*- C++ ---------*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_EVM_EVMTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_EVM_EVMTARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/MC/SectionKind.h"

namespace llvm {
class EVMTargetMachine;

/// This implementation is used for EVM ELF targets.
class EVMELFTargetObjectFile : public TargetLoweringObjectFile {
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
  MCSection *
  getExplicitSectionGlobal(const GlobalObject *GO, SectionKind Kind,
                           const TargetMachine &TM) const override { return nullptr; }

  MCSection *SelectSectionForGlobal(const GlobalObject *GO,
                                    SectionKind Kind,
                                    const TargetMachine &TM) const override {
    return getTextSection();
  }
};

} // end namespace llvm

#endif
