//===-- EVMTargetInfo.cpp - EVM Target Implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

namespace llvm {
Target &getTheEVMTarget() {
  static Target TheEVMTarget;
  return TheEVMTarget;
}
} // namespace llvm

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeEVMTargetInfo() {
  TargetRegistry::RegisterTarget(
      getTheEVMTarget(), "evm", "Ethereum Virtual Machine", "EVM",
      [](Triple::ArchType) { return false; }, true);
  RegisterTarget<Triple::evm, /*HasJIT=*/true> X(
      getTheEVMTarget(), "evm", "Ethereum Virtual Machine", "EVM");
}
