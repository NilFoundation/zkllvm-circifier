//===- AssignerISelLowering.h - Assigner DAG Lowering Interface -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Assigner uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ASSIGNER_ASSIGNERISELLOWERING_H
#define LLVM_LIB_TARGET_ASSIGNER_ASSIGNERISELLOWERING_H

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
class AssignerSubtarget;

class AssignerTargetLowering : public TargetLowering {
public:
  explicit AssignerTargetLowering(const TargetMachine &TM,
                                  const AssignerSubtarget &STI);
};
} // namespace llvm

#endif
