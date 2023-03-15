//===- AssignerTargetMachine.h - Define TargetMachine for Assigner - C++ --===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Assigner specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETMACHINE_H
#define LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETMACHINE_H

#include "AssignerSubtarget.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class AssignerTargetMachine : public LLVMTargetMachine {
  AssignerSubtarget Subtarget;

public:
  AssignerTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
                   CodeGenOpt::Level OL, bool JIT);

  const AssignerSubtarget *getSubtargetImpl() const { return &Subtarget; }
  const AssignerSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETMACHINE_H
