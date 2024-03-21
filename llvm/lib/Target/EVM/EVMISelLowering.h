//===-- EVMISelLowering.h - EVM DAG Lowering Interface ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that EVM uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_EVM_EVMISELLOWERING_H
#define LLVM_LIB_TARGET_EVM_EVMISELLOWERING_H

#include "EVM.h"

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/IR/IRBuilder.h"

namespace llvm {
class EVMSubtarget;

namespace EVMISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
#define NODE(N) N,
#include "EVMISD.def"
#undef NODE
};
}

class EVMTargetLowering : public TargetLowering {
  const EVMSubtarget &Subtarget;

public:
  explicit EVMTargetLowering(const TargetMachine &TM,
                               const EVMSubtarget &STI);

  bool CanLowerReturn(
         CallingConv::ID /*CallConv*/, MachineFunction & /*MF*/, bool /*IsVarArg*/,
         const SmallVectorImpl<ISD::OutputArg> &Outs,
         LLVMContext & /*Context*/) const override;

  bool isLegalICmpImmediate(int64_t Imm) const override;
  bool isLegalAddImmediate(int64_t Imm) const override;
  bool isTruncateFree(Type *SrcTy, Type *DstTy) const override;
  bool isTruncateFree(EVT SrcVT, EVT DstVT) const override;
  bool isZExtFree(SDValue Val, EVT VT2) const override;
  bool isSExtCheaperThanZExt(EVT SrcVT, EVT DstVT) const override;
  bool findOptimalMemOpLowering(std::vector<EVT> &MemOps, unsigned Limit,
                           const MemOp &Op, unsigned DstAS, unsigned SrcAS,
                           const AttributeList &FuncAttributes) const override {
    return false;
  }
  bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const override {
    return false;
  }

  bool isLoadReducingLegal() const override {
    return false;
  }

  bool allowsMisalignedMemoryAccesses(
      EVT, unsigned AddrSpace = 0, Align Alignment = Align(1),
      MachineMemOperand::Flags Flags = MachineMemOperand::MONone,
      bool * /*Fast*/ = nullptr) const override {
    return true;
  }

  // Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  void ReplaceNodeResults(SDNode *N, SmallVectorImpl<SDValue> &Results,
                          SelectionDAG &DAG) const override;

  // custom lowering
  SDValue LowerBR(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSIGN_EXTEND(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerFrameIndex(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerExternalSymbol(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerBasicBlock(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerCopyToReg(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerCtxz(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const;

  // This method returns the name of a target specific DAG node.
  const char *getTargetNodeName(unsigned Opcode) const override;

  std::pair<unsigned, const TargetRegisterClass *>
  getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                               StringRef Constraint, MVT VT) const override;

  MachineBasicBlock *EmitInstrWithCustomInserter(
      MachineInstr & MI, MachineBasicBlock * BB) const override;

  EVT getSetCCResultType(const DataLayout &DL, LLVMContext &Context, EVT VT)
      const override;

  SDValue getEVMCmp(SDValue LHS, SDValue RHS, ISD::CondCode CC,
                    SDValue & AVRcc, SelectionDAG & DAG, SDLoc dl) const;

  bool shouldInsertFencesForAtomic(const Instruction *I) const override {
    return isa<LoadInst>(I) || isa<StoreInst>(I);
  }
  Instruction *emitLeadingFence(IRBuilderBase &Builder, Instruction * Inst,
                                AtomicOrdering Ord) const override;
  Instruction *emitTrailingFence(IRBuilderBase &Builder, Instruction * Inst,
                                 AtomicOrdering Ord) const override;

  SDValue
  PerformDAGCombine(SDNode *N,
                    TargetLowering::DAGCombinerInfo &DCI) const override;

  MVT getScalarShiftAmountTy(const DataLayout &, EVT) const override {
    return MVT::i256;
  }

private:
  // Lower incoming arguments, copy physregs into vregs
  SDValue LowerFormalArguments(
      SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
      const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
      SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const override;
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals,
                      const SDLoc &DL, SelectionDAG &DAG) const override;
  SDValue LowerCall(TargetLowering::CallLoweringInfo & CLI,
                    SmallVectorImpl<SDValue> & InVals) const override;
  bool shouldConvertConstantLoadToIntImm(const APInt &Imm, Type *Ty)
      const override {
    return true;
  }

  bool IsEligibleForTailCallOptimization(
      CCState & CCInfo, CallLoweringInfo & CLI, MachineFunction & MF,
      const SmallVector<CCValAssign, 16> &ArgLocs) const;

  MachineBasicBlock *insertSELECTCC(MachineInstr & MI, MachineBasicBlock * BB)
      const;
};
}

#endif
