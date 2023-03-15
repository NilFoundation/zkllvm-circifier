//===-- AssignerTargetInfo.cpp - Assigner Target Implementation -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AssignerTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;
Target &llvm::getTheAssignerTarget() {
  static Target TheAssignerTarget;
  return TheAssignerTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAssignerTargetInfo() {
  RegisterTarget<Triple::assigner> X(getTheAssignerTarget(), "assigner",
                                     "zkLLVM assigner", "Assigner");
}
