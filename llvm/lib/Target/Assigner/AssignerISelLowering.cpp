//===-- AssignerISelLowering.cpp - Assigner DAG Lowering Implementation  --===//
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

#include "AssignerISelLowering.h"

using namespace llvm;

#define DEBUG_TYPE "assigner-lower"

AssignerTargetLowering::AssignerTargetLowering(const TargetMachine &TM,
                                               const AssignerSubtarget &STI)
    : TargetLowering(TM) {}
