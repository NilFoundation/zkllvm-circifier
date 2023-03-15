//===-- AssignerTargetMachine.cpp - Define TargetMachine for Assigner -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about Assigner target spec.
//
//===----------------------------------------------------------------------===//

#include "AssignerTargetMachine.h"
#include "AssignerTargetTransformInfo.h"
#include "TargetInfo/AssignerTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAssignerTarget() {
  RegisterTargetMachine<AssignerTargetMachine> X(getTheAssignerTarget());
}

static std::string computeDataLayout(const Triple &TT) {
  assert(TT.getArch() == Triple::assigner);
  // copied from x64_86:
  return "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128";
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::PIC_);
}

AssignerTargetMachine::AssignerTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   std::optional<Reloc::Model> RM,
                                   std::optional<CodeModel::Model> CM,
                                   CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      Subtarget(TT, *this) {}

TargetTransformInfo
AssignerTargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(AssignerTTIImpl(this, F));
}
