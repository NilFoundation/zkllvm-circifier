//===-- AssignerTargetTransformInfo.h - Assigner specific TTI ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file uses the target's specific information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETTRANSFORMINFO_H

#include "AssignerTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

namespace llvm {
class AssignerTTIImpl : public BasicTTIImplBase<AssignerTTIImpl> {
  typedef BasicTTIImplBase<AssignerTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const AssignerSubtarget *ST;
  const AssignerTargetLowering *TLI;

  const AssignerSubtarget *getST() const { return ST; }
  const AssignerTargetLowering *getTLI() const { return TLI; }

public:
  explicit AssignerTTIImpl(const AssignerTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  InstructionCost getMemoryOpCost(
      unsigned Opcode, Type *Src, MaybeAlign Alignment, unsigned AddressSpace,
      TTI::TargetCostKind CostKind,
      TTI::OperandValueInfo OpInfo = {TTI::OK_AnyValue, TTI::OP_None},
      const Instruction *I = nullptr) {
    return InstructionCost();
  }

  using BaseT::getVectorInstrCost;
  InstructionCost getVectorInstrCost(unsigned Opcode, Type *Val,
                                     TTI::TargetCostKind CostKind,
                                     unsigned Index, Value *Op0, Value *Op1) {
    return InstructionCost();
  }

  InstructionCost getCmpSelInstrCost(unsigned Opcode, Type *ValTy, Type *CondTy,
                                     CmpInst::Predicate VecPred,
                                     TTI::TargetCostKind CostKind,
                                     const Instruction *I = nullptr) {
    return InstructionCost();
  }

  unsigned getNumberOfParts(Type *Tp) { return 1; }
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETTRANSFORMINFO_H
