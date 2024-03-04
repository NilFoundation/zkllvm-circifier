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
  // based on x64_86:
  return "e-m:e-p:64:8-a:8-i16:8-i32:8-i64:8-v768:8-v1152:8-v1536:8";
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
