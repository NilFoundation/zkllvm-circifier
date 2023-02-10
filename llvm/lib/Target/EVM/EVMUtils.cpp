//===-- EVMUtilities.cpp - EVM Utility Functions ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements several utility functions for EVM.
///
//===----------------------------------------------------------------------===//

#include "EVMUtils.h"
#include "llvm/IR/DerivedTypes.h"

#include <sstream>

#define COMMENT_FLAG_BITSHIFT 4
#define VALUE_FLAG_BITWIDTH 4
#define VALUE_FLAG_BITSHIFT 16

namespace llvm::EVM {

uint32_t BuildCommentFlags(AsmComments commentFlag, uint16_t value) {
  return MachineInstr::TAsmComments | (commentFlag << COMMENT_FLAG_BITSHIFT) |
         (value << VALUE_FLAG_BITSHIFT);
}

void ParseCommentFlags(uint32_t input, AsmComments &commentFlag,
                            uint16_t &value) {
  assert(input & MachineInstr::TAsmComments);
  value = (input >> VALUE_FLAG_BITSHIFT);
  commentFlag = (AsmComments)((input >> COMMENT_FLAG_BITSHIFT) &
                              (1 << (VALUE_FLAG_BITWIDTH - 1)));
}

ValType valTypeFromMVT(MVT T) {
  switch (T.SimpleTy) {
  case MVT::i32:
    return ValType::I32;
  case MVT::i64:
    return ValType::I64;
  case MVT::i128:
    return ValType::I128;
  case MVT::i256:
    return ValType::I256;
  default:
    llvm_unreachable("unexpected type");
  }
}

}  // namespace llvm::EVM

