//===-- EVMISelLowering.cpp - EVM DAG Lowering Implementation  --------===//
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

#include "EVMISelLowering.h"
#include "EVM.h"
#include "EVMMachineFunctionInfo.h"
#include "EVMRegisterInfo.h"
#include "EVMSubtarget.h"
#include "EVMTargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "evm-lower"

EVMTargetLowering::EVMTargetLowering(const TargetMachine &TM,
                                         const EVMSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  // Legal register classes:
  addRegisterClass(MVT::i256, &EVM::GPRRegClass);

  // Compute derived properties from the register classes.
  computeRegisterProperties(Subtarget.getRegisterInfo());

  setBooleanContents(ZeroOrOneBooleanContent);
  setBooleanVectorContents(ZeroOrOneBooleanContent);
  setSchedulingPreference(Sched::RegPressure);
  setStackPointerRegisterToSaveRestore(EVM::SP);

  setOperationAction(ISD::CopyToReg, MVT::Other, Custom);

  for (auto VT : {MVT::i1, MVT::i8, MVT::i16, MVT::i32, MVT::i64, MVT::i128,
                  MVT::i160, MVT::i256}) {
    setOperationAction(ISD::SMIN, VT, Expand);
    setOperationAction(ISD::SMAX, VT, Expand);
    setOperationAction(ISD::UMIN, VT, Expand);
    setOperationAction(ISD::UMAX, VT, Expand);
    setOperationAction(ISD::ABS,  VT, Expand);

    // we don't have complex operations.
    setOperationAction(ISD::ADDC, VT, Expand);
    setOperationAction(ISD::SUBC, VT, Expand);
    setOperationAction(ISD::ADDE, VT, Expand);
    setOperationAction(ISD::SUBE, VT, Expand);
    setOperationAction(ISD::ADDCARRY, VT, Expand);
    setOperationAction(ISD::SUBCARRY, VT, Expand);
    setOperationAction(ISD::SADDO, VT, Expand);
    setOperationAction(ISD::UADDO, VT, Expand);
    setOperationAction(ISD::SSUBO, VT, Expand);
    setOperationAction(ISD::USUBO, VT, Expand);
    setOperationAction(ISD::SMULO, VT, Expand);
    setOperationAction(ISD::UMULO, VT, Expand);
    setOperationAction(ISD::SADDSAT, VT, Expand);
    setOperationAction(ISD::UADDSAT, VT, Expand);
    setOperationAction(ISD::SSUBSAT, VT, Expand);
    setOperationAction(ISD::USUBSAT, VT, Expand);
    setOperationAction(ISD::SMULFIX, VT, Expand);
    setOperationAction(ISD::UMULFIX, VT, Expand);

    setOperationAction(ISD::SDIVREM, VT, Expand);
    setOperationAction(ISD::UDIVREM, VT, Expand);
    setOperationAction(ISD::MULHU, VT, Expand);
    setOperationAction(ISD::MULHS, VT, Expand);
    setOperationAction(ISD::UMUL_LOHI, VT, Expand);
    setOperationAction(ISD::SMUL_LOHI, VT, Expand);
    setOperationAction(ISD::ROTR, VT, Expand);
    setOperationAction(ISD::ROTL, VT, Expand);
    setOperationAction(ISD::SHL_PARTS, VT, Expand);
    setOperationAction(ISD::SRL_PARTS, VT, Expand);
    setOperationAction(ISD::SRA_PARTS, VT, Expand);
    setOperationAction(ISD::SELECT, VT, Expand);

    setOperationAction(ISD::SELECT_CC, VT, Custom);
    setOperationAction(ISD::SETCC, VT, Custom);

    setOperationAction(ISD::BSWAP, VT, Expand);
    setOperationAction(ISD::BITREVERSE, VT, Expand);
    setOperationAction(ISD::CTTZ, VT, Custom);
    setOperationAction(ISD::CTLZ, VT, Custom);
    setOperationAction(ISD::CTPOP, VT, Legal);
    setOperationAction(ISD::CTTZ_ZERO_UNDEF, VT, Custom);
    setOperationAction(ISD::CTLZ_ZERO_UNDEF, VT, Custom);
    setOperationAction(ISD::SIGN_EXTEND_INREG, VT, Custom);

    setOperationAction(ISD::GlobalAddress, VT, Custom);
    setOperationAction(ISD::ExternalSymbol, VT, Custom);
    setOperationAction(ISD::BlockAddress, VT, Expand);

    // FIXME: DYNAMIC_STACKALLOC
    setOperationAction(ISD::DYNAMIC_STACKALLOC, VT, Expand);
  }
  setOperationAction(ISD::BR_CC, MVT::i256, Custom);
  setOperationAction(ISD::BR_JT, MVT::Other, Expand);

  // custom lowering the branch 
  setOperationAction(ISD::BRCOND, MVT::Other, Expand);

  setOperationAction(ISD::FrameIndex, MVT::i256, Custom);
  setOperationAction(ISD::GlobalAddress, MVT::i256, Custom);

  setOperationAction(ISD::STACKSAVE, MVT::Other, Expand);
  setOperationAction(ISD::STACKRESTORE, MVT::Other, Expand);

  // extends
  setOperationAction(ISD::ANY_EXTEND,  MVT::i256, Expand);
  setOperationAction(ISD::ZERO_EXTEND, MVT::i256, Expand);
  setOperationAction(ISD::SIGN_EXTEND, MVT::i256, Custom);

  // we don't have trunc stores.
  setTruncStoreAction(MVT::i256, MVT::i8,   Legal);
  setTruncStoreAction(MVT::i256, MVT::i16,  Legal);
  setTruncStoreAction(MVT::i256, MVT::i32,  Legal);
  setTruncStoreAction(MVT::i256, MVT::i64,  Legal);
  setTruncStoreAction(MVT::i256, MVT::i128, Legal);
  setTruncStoreAction(MVT::i256, MVT::i160, Legal);

  // Load extented operations for i1 types must be promoted
  for (MVT VT : MVT::integer_valuetypes()) {
    setLoadExtAction(ISD::EXTLOAD,  VT, MVT::i1,  Promote);
    setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i1,  Promote);
    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i1,  Promote);
  }

  setTargetDAGCombine({ISD::SIGN_EXTEND, ISD::ZERO_EXTEND, ISD::ANY_EXTEND});
}

EVT EVMTargetLowering::getSetCCResultType(const DataLayout &DL, LLVMContext &,
                                          EVT VT) const {
  return MVT::i256;
}

bool EVMTargetLowering::isLegalICmpImmediate(int64_t Imm) const {
  return true;
}

bool EVMTargetLowering::isLegalAddImmediate(int64_t Imm) const {
  return true;
}

bool EVMTargetLowering::isTruncateFree(Type *SrcTy, Type *DstTy) const {
  return true;
}

bool EVMTargetLowering::isTruncateFree(EVT SrcVT, EVT DstVT) const {
  return true;
}

bool EVMTargetLowering::isZExtFree(SDValue Val, EVT VT2) const {
  return false;
}

bool EVMTargetLowering::isSExtCheaperThanZExt(EVT SrcVT, EVT DstVT) const {
  return true;
}

/*
static EVMISD::NodeType getReverseCmpOpcode(ISD::CondCode CC) {
  switch (CC) {
    default:
      llvm_unreachable("unimplemented condition code.");
      break;
    case ISD::SETLE:
      return EVMISD::SGT;
    case ISD::SETGE:
      return EVMISD::SLT;
    case ISD::SETULE:
      return EVMISD::GT;
    case ISD::SETUGE:
      return EVMISD::LT;
  }
}
*/

SDValue EVMTargetLowering::LowerFrameIndex(SDValue Op,
                                           SelectionDAG &DAG) const {
    unsigned FI = cast<FrameIndexSDNode>(Op)->getIndex();

    // Record the FI so that we know how many frame slots are allocated to
    // frames.
    return DAG.getTargetFrameIndex(FI, Op.getValueType());
}

SDValue
EVMTargetLowering::LowerGlobalAddress(SDValue Op,
                                      SelectionDAG &DAG) const {
  SDLoc DL(Op);
  const auto *GA = cast<GlobalAddressSDNode>(Op);
  EVT VT = Op.getValueType();
  assert(GA->getTargetFlags() == 0 &&
         "Unexpected target flags on generic GlobalAddressSDNode");

  if (GA->getAddressSpace() != 0) {
    llvm_unreachable("multiple address space unimplemented");
  }

  return DAG.getNode(EVMISD::WRAPPER, DL, VT,
                     DAG.getTargetGlobalAddress(GA->getGlobal(),
                                                DL, VT,
                                                GA->getOffset()));
}

SDValue
EVMTargetLowering::LowerExternalSymbol(SDValue Op,
                                       SelectionDAG &DAG) const {
  SDLoc DL(Op);
  const auto *ES = cast<ExternalSymbolSDNode>(Op);
  EVT VT = Op.getValueType();
  assert(ES->getTargetFlags() == 0 &&
         "Unexpected target flags on generic ExternalSymbolSDNode");

  return DAG.getNode(
      EVMISD::WRAPPER, DL, VT,
      DAG.getTargetExternalSymbol(ES->getSymbol(), MVT::i256));
}

SDValue
EVMTargetLowering::LowerBlockAddress(SDValue Op,
                                     SelectionDAG &DAG) const {
  SDLoc dl(Op);
  auto PtrVT = getPointerTy(DAG.getDataLayout());
  const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();
  SDValue Result = DAG.getTargetBlockAddress(BA, PtrVT);

  return DAG.getNode(EVMISD::WRAPPER, dl, PtrVT, Result);
}

SDValue EVMTargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  SDLoc DL(Op);

  return DAG.getNode(
      EVMISD::BRCC, DL, Op.getValueType(), Chain, LHS, RHS,
      DAG.getConstant(CC, DL, LHS.getValueType()), Dest);
}

SDValue EVMTargetLowering::LowerBR(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  SDValue Dest = Op.getOperand(1);
  SDLoc DL(Op);

  return DAG.getNode(ISD::BRIND, DL, Op.getValueType(), Chain, Dest);
}

SDValue EVMTargetLowering::LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  SDValue TrueV = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDLoc DL(Op);

  // Extend operands in case they are smaller than 256-bits.
  // If we don't do this extension, Type Legalizing would fail.
  if (LHS.getValueType().getSizeInBits() < 256) {
    LHS = DAG.getNode(ISD::ZERO_EXTEND, SDLoc(LHS), {MVT::i256}, LHS);
  }
  if (RHS.getValueType().getSizeInBits() < 256) {
    RHS = DAG.getNode(ISD::ZERO_EXTEND, SDLoc(RHS), {MVT::i256}, RHS);
  }

  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue TargetCC = DAG.getConstant(CC, DL, MVT::i256);
  SDValue Ops[] = {LHS, RHS, TargetCC, TrueV, FalseV};

  return DAG.getNode(EVMISD::SELECTCC, DL, VTs, Ops);
}

SDValue EVMTargetLowering::LowerSIGN_EXTEND(SDValue Op, SelectionDAG &DAG) const {
  SDValue Op0 = Op.getOperand(0);
  SDLoc dl(Op);
  assert(Op.getValueType() == MVT::i256 && "Unhandled target sign_extend.");

  if (Op0.getValueType().getSizeInBits() < 256) {
    Op0 = DAG.getNode(ISD::ZERO_EXTEND, SDLoc(Op0), {MVT::i256}, Op0);
  }

  auto OpSizeInBits = cast<VTSDNode>(Op.getOperand(1))->getVT().getSizeInBits();
  // FIXME(!): Comment this case
//  if (OpSizeInBits == 1) {
//    return DAG.getNode(ISD::AND, dl, MVT::i256, Op0, DAG.getConstant(1, dl, MVT::i256));
//  }
  unsigned Width = OpSizeInBits / 8;
  assert(Width != 0);

  // According to EVM spec, size of extended value is "byte width - 1"
  return DAG.getNode(EVMISD::SIGNEXTEND, dl, MVT::i256,
                     DAG.getConstant(Width - 1, dl, MVT::i256), Op0);
}

SDValue EVMTargetLowering::LowerOperation(SDValue Op,
                                          SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
#ifndef NDEBUG
    errs() << "Unexpected node: "; Op.dump(&DAG);
#endif
    llvm_unreachable("unimplemented lowering operation,");
  case ISD::BR:
    return LowerBR(Op, DAG);
  case ISD::BR_CC:
    return LowerBR_CC(Op, DAG);
  case ISD::SELECT_CC:
    return LowerSELECT_CC(Op, DAG);
  case ISD::FrameIndex:
    return LowerFrameIndex(Op, DAG);
  case ISD::SIGN_EXTEND:
  case ISD::SIGN_EXTEND_INREG:
    return LowerSIGN_EXTEND(Op, DAG);
  case ISD::GlobalAddress:
    return LowerGlobalAddress(Op, DAG);
  case ISD::ExternalSymbol:
    return LowerExternalSymbol(Op, DAG);
  case ISD::BlockAddress:
    return LowerBlockAddress(Op, DAG);
  case ISD::CopyToReg:
    return LowerCopyToReg(Op, DAG);
  case ISD::SETCC: {
    SDValue LHS = Op.getOperand(0);
    SDValue RHS = Op.getOperand(1);
    ConstantSDNode *LHSConst = dyn_cast<ConstantSDNode>(LHS);
    ConstantSDNode *RHSConst = dyn_cast<ConstantSDNode>(LHS);

    if (!LHSConst && LHS.getValueType().getSizeInBits() < 256) {
      auto Extend = (LHS.getValueType().getSizeInBits() == 1) ?
                    ISD::ZERO_EXTEND : ISD::SIGN_EXTEND;
      LHS = DAG.getNode(Extend, SDLoc(LHS), {MVT::i256}, LHS);
    }
    if (!RHSConst && RHS.getValueType().getSizeInBits() < 256) {
      auto Extend = (RHS.getValueType().getSizeInBits() == 1) ?
                    ISD::ZERO_EXTEND : ISD::SIGN_EXTEND;
      RHS = DAG.getNode(Extend, SDLoc(RHS), {MVT::i256}, RHS);
    }
    auto Res = DAG.getNode(Op.getOpcode(), SDLoc(Op), MVT::i256, LHS, RHS,
                       Op.getOperand(2));
    // Use truncate at the end to make common legalizer happy. Otherwise, it
    // can't find our new legalized node when it legalizes the operands of
    // arithmetic instructions.
    return DAG.getNode(ISD::TRUNCATE, SDLoc(Op), Op.getValueType(), Res);
  }
  case ISD::CTLZ:
  case ISD::CTTZ:
  case ISD::CTLZ_ZERO_UNDEF:
  case ISD::CTTZ_ZERO_UNDEF:
    return LowerCtxz(Op, DAG);
  }
}

SDValue EVMTargetLowering::LowerCtxz(SDValue Op, SelectionDAG &DAG) const {
  auto Node = Op.getNode();
  TargetLowering::ArgListTy Args;
  TargetLowering::ArgListEntry Entry;
  for (const SDValue &Op : Node->op_values()) {
    EVT ArgVT = Op.getValueType();
    Type *ArgTy = ArgVT.getTypeForEVT(*DAG.getContext());
    Entry.Node = Op;
    Entry.Ty = ArgTy;
    if (Op.getValueSizeInBits() != 256) {
      Entry.Node = DAG.getNode(ISD::SIGN_EXTEND, SDLoc(Op), {MVT::i256}, Op);
    }
    Args.push_back(Entry);
  }
  const char* CalleeStr;
  auto OperandSize = Op.getOperand(0).getValueSizeInBits();
  if (Op.getOpcode() == ISD::CTLZ_ZERO_UNDEF || Op.getOpcode() == ISD::CTLZ) {
    CalleeStr = (OperandSize == 64) ? "__evm_builtin_clzll"
                                    : "__evm_builtin_clz";
  } else {
    CalleeStr = (OperandSize == 64) ? "__evm_builtin_ctzll"
                                    : "__evm_builtin_ctz";
  }
  TargetLowering::CallLoweringInfo CLI(DAG);
  CLI.setDebugLoc(SDLoc(Op.getNode()))
      .setChain(DAG.getEntryNode())
      .setLibCallee(CallingConv::C, Node->getValueType(0).getTypeForEVT(*DAG.getContext()),
                    DAG.getExternalSymbol(
                        CalleeStr, getPointerTy(DAG.getDataLayout())),
                    std::move(Args));
  LowerCallTo(CLI);

  assert(CLI.InVals.size() == 1);

  SDValue ResNode = CLI.InVals[0];
  if (ResNode.getValueSizeInBits() != Op.getValueSizeInBits()) {
    ResNode = DAG.getZExtOrTrunc(CLI.InVals[0], SDLoc(Op), Op.getValueType());
  }
  DAG.ReplaceAllUsesOfValueWith(SDValue(Node, 0), ResNode);

  return ResNode;
}

SDValue EVMTargetLowering::LowerCopyToReg(SDValue Op,
                                          SelectionDAG &DAG) const {
  SDValue Src = Op.getOperand(2);
  if (isa<FrameIndexSDNode>(Src.getNode())) {
    SDValue Chain = Op.getOperand(0);
    SDLoc DL(Op);
    Register Reg = cast<RegisterSDNode>(Op.getOperand(1))->getReg();
    EVT VT = Src.getValueType();
    SDValue Copy(DAG.getMachineNode(EVM::pMOVE_r, DL, VT, Src),
                 0);
    return Op.getNode()->getNumValues() == 1
               ? DAG.getCopyToReg(Chain, DL, Reg, Copy)
               : DAG.getCopyToReg(Chain, DL, Reg, Copy,
                                  Op.getNumOperands() == 4 ? Op.getOperand(3)
                                                           : SDValue());
  }
  return SDValue();
}

void EVMTargetLowering::ReplaceNodeResults(SDNode *N,
                                           SmallVectorImpl<SDValue> &Results,
                                           SelectionDAG &DAG) const {
  SDLoc DL(N);

  assert(N->getNumValues() == 1);

  auto Result = LowerOperation(SDValue(N, 0), DAG);

  Results.push_back(Result);
}

MachineBasicBlock *
EVMTargetLowering::insertSELECTCC(MachineInstr &MI,
                                  MachineBasicBlock *MBB) const {
  const EVMInstrInfo &TII = (const EVMInstrInfo &)*MI.getParent()
                                ->getParent()
                                ->getSubtarget()
                                .getInstrInfo();
  DebugLoc dl = MI.getDebugLoc();

  // To "insert" a SELECT instruction, we insert the diamond
  // control-flow pattern. The incoming instruction knows the
  // destination vreg to set, the condition code register to branch
  // on, the true/false values to select between, and a branch opcode
  // to use.

  MachineFunction *MF = MBB->getParent();
  const BasicBlock *LLVM_BB = MBB->getBasicBlock();
  MachineBasicBlock *FallThrough = MBB->getFallThrough();

  // If the current basic block falls through to another basic block,
  // we must insert an unconditional branch to the fallthrough destination
  // if we are to insert basic blocks at the prior fallthrough point.
  if (FallThrough != nullptr) {
    BuildMI(MBB, dl, TII.get(EVM::pJUMPTO_r)).addMBB(FallThrough);
  }

  // Move pSTACKARG_r instructions before SELECTCC, thereby after we split
  // basic block, they appear in the entry block.
  {
    SmallVector<MachineInstr *, 4> StackArgs;
    for (auto It = MI.getIterator(); It != MBB->end(); ++It) {
      if (It->getOpcode() == EVM::pSTACKARG_r) {
        StackArgs.push_back(&*It);
      }
    }
    for (auto SAI : StackArgs) {
      MBB->remove(SAI);
      MBB->insertAfter(MI.getPrevNode()->getIterator(), SAI);
    }
  }

  // create two MBBs to handle true and false
  MachineBasicBlock *trueMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *falseMBB = MF->CreateMachineBasicBlock(LLVM_BB);

  MachineFunction::iterator I;
  for (I = MF->begin(); I != MF->end() && &(*I) != MBB; ++I);
  if (I != MF->end()) ++I;
  MF->insert(I, falseMBB);
  MF->insert(I, trueMBB);

  // Transfer remaining instructions and all successors of the current
  // block to the block which will contain the Phi node for the
  // select.
  trueMBB->splice(trueMBB->begin(), MBB,
                  std::next(MachineBasicBlock::iterator(MI)), MBB->end());
  trueMBB->transferSuccessorsAndUpdatePHIs(MBB);

  MachineRegisterInfo &RegInfo = MF->getRegInfo();
  const TargetRegisterClass *RC = getRegClassFor(MVT::i256);

  unsigned SelectedValue = MI.getOperand(0).getReg();
  unsigned LHS = MI.getOperand(1).getReg();
  unsigned RHS = MI.getOperand(2).getReg();
  unsigned TrueReg;
  if (MI.getOperand(4).isReg()) {
    TrueReg = MI.getOperand(4).getReg();
  } else if (MI.getOperand(4).isFI()) {
    TrueReg = RegInfo.createVirtualRegister(RC);
    BuildMI(*MBB, MI, dl, TII.get(EVM::PUSH32_r), TrueReg)
        .addFrameIndex(MI.getOperand(4).getIndex());
  }
  unsigned FalseReg;
  if (MI.getOperand(5).isReg()) {
    FalseReg = MI.getOperand(5).getReg();
  } else if (MI.getOperand(5).isFI()) {
    FalseReg = RegInfo.createVirtualRegister(RC);
    BuildMI(*MBB, MI, dl, TII.get(EVM::PUSH32_r), FalseReg)
        .addFrameIndex(MI.getOperand(5).getIndex());
  }
  // construct conditional jump
  {
    unsigned rvreg = RegInfo.createVirtualRegister(RC);

    int CC = MI.getOperand(3).getCImm()->getZExtValue();
    switch (CC) {
      default:
        llvm_unreachable("unimplemented.");
      case ISD::SETEQ:
        BuildMI(MBB, dl, TII.get(EVM::EQ_r), rvreg).addReg(LHS).addReg(RHS);
        break;
      case ISD::SETNE:
        {
          unsigned reg = RegInfo.createVirtualRegister(RC);
          BuildMI(MBB, dl, TII.get(EVM::EQ_r), reg).addReg(LHS).addReg(RHS);
          BuildMI(MBB, dl, TII.get(EVM::ISZERO_r), rvreg).addReg(reg);
        }
        break;
      case ISD::SETLT:
        BuildMI(MBB, dl, TII.get(EVM::SLT_r), rvreg).addReg(LHS).addReg(RHS);
        break;
      case ISD::SETLE:
        {
          unsigned reg = RegInfo.createVirtualRegister(RC);
          BuildMI(MBB, dl, TII.get(EVM::SGT_r), reg).addReg(LHS).addReg(RHS);
          BuildMI(MBB, dl, TII.get(EVM::ISZERO_r), rvreg).addReg(reg);
        }
        break;
      case ISD::SETGT:
        BuildMI(MBB, dl, TII.get(EVM::SGT_r), rvreg).addReg(LHS).addReg(RHS);
        break;
      case ISD::SETGE:
        {
          unsigned reg = RegInfo.createVirtualRegister(RC);
          BuildMI(MBB, dl, TII.get(EVM::SLT_r), reg).addReg(LHS).addReg(RHS);
          BuildMI(MBB, dl, TII.get(EVM::ISZERO_r), rvreg).addReg(reg);
        }
        break;
      case ISD::SETULT:
        BuildMI(MBB, dl, TII.get(EVM::LT_r), rvreg).addReg(LHS).addReg(RHS);
        break;
      case ISD::SETULE:
        {
          unsigned reg = RegInfo.createVirtualRegister(RC);
          BuildMI(MBB, dl, TII.get(EVM::GT_r), reg).addReg(LHS).addReg(RHS);
          BuildMI(MBB, dl, TII.get(EVM::ISZERO_r), rvreg).addReg(reg);
        }
        break;
      case ISD::SETUGT:
        BuildMI(MBB, dl, TII.get(EVM::GT_r), rvreg).addReg(LHS).addReg(RHS);
        break;
      case ISD::SETUGE:
        {
          unsigned reg = RegInfo.createVirtualRegister(RC);
          BuildMI(MBB, dl, TII.get(EVM::LT_r), reg).addReg(LHS).addReg(RHS);
          BuildMI(MBB, dl, TII.get(EVM::ISZERO_r), rvreg).addReg(reg);
        }
        break;
    }

    BuildMI(MBB, dl, TII.get(EVM::pJUMPIF_r)).addReg(rvreg).addMBB(trueMBB);
  }

  // Finally, add branch to falseMBB 
  BuildMI(MBB, dl, TII.get(EVM::pJUMPTO_r)).addMBB(falseMBB);

  MBB->addSuccessor(falseMBB);
  MBB->addSuccessor(trueMBB);

  // Unconditionally flow back to the true block
  {
    BuildMI(falseMBB, dl, TII.get(EVM::pJUMPTO_r)).addMBB(trueMBB);
    falseMBB->addSuccessor(trueMBB);
  }

  // Set up the Phi node to determine where we came from
  BuildMI(*trueMBB, trueMBB->begin(), dl, TII.get(EVM::PHI), SelectedValue)
    .addReg(TrueReg).addMBB(MBB)
    .addReg(FalseReg).addMBB(falseMBB);

  MI.eraseFromParent(); // The pseudo instruction is gone now.
  return trueMBB;
}

MachineBasicBlock *
EVMTargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                               MachineBasicBlock *MBB) const {
  int Opc = MI.getOpcode();

  switch (Opc) {
    case EVM::Selectcc :
      return insertSELECTCC(MI, MBB);
    default:
      llvm_unreachable("unimplemented.");
  }
}

#include "EVMGenCallingConv.inc"

// Transform physical registers into virtual registers.
SDValue EVMTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  MachineFunction &MF = DAG.getMachineFunction();
  EVMMachineFunctionInfo *MFI = MF.getInfo<EVMMachineFunctionInfo>();

  // Instantiate virtual registers for each of the incoming value.
  // unused register will be set to UNDEF.
  SmallVector<SDValue, 16> ArgsChain;
  ArgsChain.push_back(Chain);

  // record the number of stack args.
  MFI->setNumStackArgs(Ins.size());

  for (const ISD::InputArg &In __attribute__((unused)) : Ins) {
    SmallVector<SDValue, 4> Opnds;

    // (top) stackarg0(1st arg), stackarg1 (2nd arg), ... (bottom)
    // the index starts with 0. the zero index is left for return address
    const SDValue &idx = DAG.getTargetConstant(InVals.size(),
                                               DL, MVT::i64);
    Opnds.push_back(idx);

    const SDValue &StackArg =
       DAG.getNode(EVMISD::STACKARG, DL, MVT::i256, Opnds);

    InVals.push_back(StackArg);
  }

  return Chain;
}

/// IsEligibleForTailCallOptimization - Check whether the call is eligible
/// for tail call optimization.
/// Note: This is modelled after ARM's IsEligibleForTailCallOptimization.
bool EVMTargetLowering::IsEligibleForTailCallOptimization(
  CCState &CCInfo, CallLoweringInfo &CLI, MachineFunction &MF,
  const SmallVector<CCValAssign, 16> &ArgLocs) const {
  return false;
}

SDValue EVMTargetLowering::LowerCall(CallLoweringInfo &CLI,
                                     SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  auto &Outs = CLI.Outs;
  auto &Ins = CLI.Ins;
  auto &OutVals = CLI.OutVals;
  SDLoc DL = CLI.DL;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  MachineFunction &MF = DAG.getMachineFunction();
  auto Layout = MF.getDataLayout();
  bool IsVarArg = CLI.IsVarArg;

  CallingConv::ID CallConv = CLI.CallConv;


  if (IsVarArg) { llvm_unreachable("unimplemented."); }

  // For now we do not specifically optimize for tail calls.
  CLI.IsTailCall = false;

  switch (CallConv) {
  default:
    report_fatal_error("Unsupported calling convention");
  case CallingConv::Fast:
  case CallingConv::C:
    break;
  }

  if (Ins.size() > 1) {
    llvm_unreachable("unimplemented.");
  }

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_EVM);

  // Insert callseq start
  unsigned NumBytes = CCInfo.getNextStackOffset();
  auto PtrVT = getPointerTy(MF.getDataLayout());
  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, DL);

  // Compute the operands for the CALLn node.
  SmallVector<SDValue, 16> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // TODO: remove frame manipulation
  for (unsigned I = 0; I < Outs.size(); ++I) {
    const ISD::OutputArg &Out = Outs[I];
    SDValue &OutVal = OutVals[I];
    if (Out.Flags.isByVal() && Out.Flags.getByValSize() != 0) {
      auto &MFI = MF.getFrameInfo();
      int FI = MFI.CreateStackObject(Out.Flags.getByValSize(),
          Out.Flags.getNonZeroByValAlign(), false);
      SDValue SizeNode =
        DAG.getConstant(Out.Flags.getByValSize(), DL, MVT::i32);
      SDValue FINode = DAG.getFrameIndex(FI, getPointerTy(Layout));
      Chain = DAG.getMemcpy(
          Chain, DL, FINode, OutVal, SizeNode, Out.Flags.getNonZeroByValAlign(),
          false, false,
          false, MachinePointerInfo(), MachinePointerInfo());
      OutVal = FINode;
    }
    //Ops.push_back(OutVal);
  }

  // PC + 6 is the return address
  // insert the first operand to Chain
  //SDValue PC = DAG.getNode(EVMISD::PC_PLUS_OFFSET, DL, MVT::i256);

  // Add all fixed arguments. Note that for non-varargs calls, NumFixedArgs
  // isn't reliable.
  //Ops.push_back(PC);
  Ops.append(OutVals.begin(), OutVals.end());


  SmallVector<EVT, 8> InTys;
  for (const auto &In : Ins) {
    InTys.push_back(In.VT);
  }
  InTys.push_back(MVT::Other);
  SDVTList InTyList = DAG.getVTList(InTys);

  unsigned opc = Ins.empty() ? EVMISD::CALLVOID : EVMISD::CALL;
  SDValue Res = DAG.getNode(opc, DL, InTyList, Ops);

  if (Ins.empty()) {
    Chain = Res;
  } else {
    InVals.push_back(Res);
    Chain = Res.getValue(1);
  }



  Chain = DAG.getCALLSEQ_END(
            Chain,
            DAG.getConstant(NumBytes, DL, PtrVT, true),
            DAG.getConstant(0, DL, PtrVT, true),
            opc == EVMISD::CALLVOID ? Chain.getValue(0) : Chain.getValue(1),
            DL);

  return Chain;
}

bool EVMTargetLowering::CanLowerReturn(
    CallingConv::ID /*CallConv*/, MachineFunction & /*MF*/, bool /*IsVarArg*/,
    const SmallVectorImpl<ISD::OutputArg> &Outs,
    LLVMContext & /*Context*/) const {
  // We can't currently handle returning tuples.
  return Outs.size() <= 1;
}

SDValue
EVMTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
                               const SmallVectorImpl<SDValue> &OutVals,
                               const SDLoc &DL, SelectionDAG &DAG) const {
  assert(Outs.size() <= 1 && "EVM can only return up to one value");

  SmallVector<SDValue, 4> RetOps(1, Chain);
  RetOps.append(OutVals.begin(), OutVals.end());

  Chain = DAG.getNode(EVMISD::RET_FLAG, DL, MVT::Other, RetOps);

  return Chain;
}

const char *EVMTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (static_cast<EVMISD::NodeType>(Opcode)) {
  case EVMISD::FIRST_NUMBER:
    break;
#define NODE(N)                                                  \
  case EVMISD::N:                                                \
    return "EVMISD::" #N;
#include "EVMISD.def"
#undef NODE
  }
  return nullptr;
}

std::pair<unsigned, const TargetRegisterClass *>
EVMTargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                StringRef Constraint,
                                                MVT VT) const {
  llvm_unreachable("unimplemented.");
}

Instruction *EVMTargetLowering::emitLeadingFence(IRBuilderBase &Builder,
                                                 Instruction *Inst,
                                                 AtomicOrdering Ord) const {
  llvm_unreachable("unimplemented.");
  return nullptr;
}

Instruction *EVMTargetLowering::emitTrailingFence(IRBuilderBase &Builder,
                                                  Instruction *Inst,
                                                  AtomicOrdering Ord) const {
  llvm_unreachable("unimplemented.");
  return nullptr;
}

SDValue EVMTargetLowering::PerformDAGCombine(SDNode *N,
                  TargetLowering::DAGCombinerInfo &DCI) const {
  if (N->getOpcode() == ISD::ZERO_EXTEND || N->getOpcode() == ISD::SIGN_EXTEND
      || N->getOpcode() == ISD::ANY_EXTEND) {
    if (N->getValueSizeInBits(0) == N->getOperand(0).getValueSizeInBits()) {
      DCI.DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), N->getOperand(0));
    }
  }
  return SDValue();
}