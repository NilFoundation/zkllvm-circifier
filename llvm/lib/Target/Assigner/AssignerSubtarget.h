//===- AssignerSubtarget.h - Define Subtarget for the Assigner --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Assigner specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ASSIGNER_ASSIGNERSUBTARGET_H
#define LLVM_LIB_TARGET_ASSIGNER_ASSIGNERSUBTARGET_H

#include "AssignerISelLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class StringRef;

class AssignerSubtarget : public TargetSubtargetInfo {
  AssignerTargetLowering TLInfo;

public:
  AssignerSubtarget(const Triple &TT, const TargetMachine &TM);

  const AssignerTargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
};
} // namespace llvm

#endif
