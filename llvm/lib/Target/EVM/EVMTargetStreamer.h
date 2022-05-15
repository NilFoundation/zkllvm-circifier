//===- EVMTargetStreamer.h - EVM Target Streamer ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_EVM_EVMTARGETSTREAMER_H
#define LLVM_LIB_TARGET_EVM_EVMTARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCInst.h"

namespace llvm {

class EVMTargetStreamer : public MCTargetStreamer {
  EVMTargetStreamer(MCStreamer &S);
  ~EVMTargetStreamer() override;

  void EmitInstruction(MCInst &Inst) {
    // Scan for values.
    for (unsigned i = Inst.getNumOperands(); i--;)
      if (Inst.getOperand(i).isExpr())
        this->Streamer.visitUsedExpr(*Inst.getOperand(i).getExpr());
  }
};

class EVMJsonTargetStreamer : public EVMTargetStreamer {
public:
  EVMJsonTargetStreamer(MCStreamer &S);
  ~EVMJsonTargetStreamer() override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_EVM_EVMTARGETSTREAMER_H
