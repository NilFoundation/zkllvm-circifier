//===-- EVMSubtarget.cpp - EVM Subtarget Information ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the EVM specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "EVMSubtarget.h"
#include "EVM.h"
#include "EVMFrameLowering.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "evm-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "EVMGenSubtargetInfo.inc"

void EVMSubtarget::anchor() {}

EVMSubtarget &EVMSubtarget::initializeSubtargetDependencies(StringRef CPU,
                                                            StringRef FS) {
  // Determine default and user-specified characteristics
  return *this;
}

EVMSubtarget::EVMSubtarget(const Triple &TT, StringRef CPU,
                           StringRef FS, const TargetMachine &TM)
    : EVMGenSubtargetInfo(TT, CPU, CPU, FS), AllocatedGlobalSlots(0),
      FrameLowering(initializeSubtargetDependencies(CPU, FS)), InstrInfo(),
      TLInfo(TM, *this) {}
