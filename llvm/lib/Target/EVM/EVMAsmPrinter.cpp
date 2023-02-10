//===-- EVMAsmPrinter.cpp - EVM LLVM assembly writer ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the EVM assembly language.
//
//===----------------------------------------------------------------------===//

#include "EVM.h"
#include "EVMMCInstLower.h"
#include "MCTargetDesc/EVMInstPrinter.h"
#include "EVMTargetMachine.h"
#include "EVMUtils.h"
#include "llvm/CodeGen/Analysis.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCSectionEVM.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

#include "llvm/MC/MCAssembler.h"

#include "MCTargetDesc/EVMTargetStreamer.h"

#include <optional>
using namespace llvm;
using namespace EVM;

#define DEBUG_TYPE "evm-asm-printer"

namespace {
class EVMAsmPrinter : public AsmPrinter {
public:
  explicit EVMAsmPrinter(TargetMachine &TM,
                         std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  //void EmitFunctionEntryLabel() override;

  StringRef getPassName() const override { return "EVM Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;

  void emitConstantPool() override;

  void emitXXStructorList(const DataLayout &DL, const Constant *List,
                          bool IsCtor) override;

  bool emitBigInt(const ConstantInt *CI) override;

  void printOperand(const MachineInstr *MI, unsigned OpNo, raw_ostream &OS);

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       const char *ExtraCode, raw_ostream &OS) override;
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                             const char *ExtraCode, raw_ostream &OS) override;

  // void EmitToStreamer(MCStreamer &S, const MCInst &Inst);
  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI);

  // Wrapper needed for tblgenned pseudo lowering.
  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp) const {
    return LowerEVMMachineOperandToMCOperand(MO, MCOp, *this);
    }
  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  MCSymbol* createSymbol(std::string name) const;
  MachineFunction* CurrentFunc{nullptr};

  std::unique_ptr<MCSubtargetInfo> STI;
  MCContext* ctx;
};
}

bool EVMAsmPrinter::runOnMachineFunction(MachineFunction &MF) {
  auto Section = OutStreamer->getContext().getEVMSection(".text");
  auto& Fragment = cast<MCDataFragment>(*Section->getFragmentList().begin());
  unsigned offset = Fragment.getContents().size();

  auto &F = MF.getFunction();

  CurrentFunc = &MF;

  auto Sym = static_cast<MCSymbolEVM*>(getSymbol(&F));
  bool isEvm = F.getAttributes().hasFnAttr(Attribute::EvmFunc);
  Sym->setKind(MCSymbolEVM::KindFunction);
  Sym->setOffset(Fragment.getContents().size());
  if (isEvm) {
    Sym->setHidden(false);
    F.setVisibility(llvm::GlobalValue::DefaultVisibility);
  } else if (!F.hasLocalLinkage()) {
    Sym->setHidden(true);
    F.setVisibility(llvm::GlobalValue::HiddenVisibility);
  }


  // TODO: currently we allow only integer types in functions' parameters.
  SetupMachineFunction(MF);
  emitFunctionBody();

  Section->addFunction(&MF.getFunction(), offset,
                       Fragment.getContents().size() - offset);

  Sym->setSize(Fragment.getContents().size() - Sym->getOffset());

  return false;
}

MCSymbol* EVMAsmPrinter::createSymbol(std::string name) const {
  assert(!ctx->lookupSymbol(name));
  return ctx->getOrCreateSymbol(name);
}

static bool emitEVMComments(const MachineInstr* MI, raw_ostream &OS) {
  if (!MI->getAsmPrinterFlag(MachineInstr::TAsmComments)) {
    return false;
  }
  uint32_t flags = MI->getAsmPrinterFlags();

  EVM::AsmComments commentType;
  uint16_t commentValue;
  EVM::ParseCommentFlags(flags, commentType, commentValue);

  switch (commentType) {
  case EVM::PUTLOCAL:
    OS << "putlocal: " << commentValue << "\n";
    break;
  case EVM::GETLOCAL:
    OS << "getlocal: " << commentValue << "\n";
    break;
  case EVM::SUBROUTINE_BEGIN:
    OS << "subroutine call\n";
    break;
  case EVM::RETURN_FROM_SUBROUTINE:
    OS << "return\n";
    break;
  default:
    llvm_unreachable("unimplemented");
  }

  return true;
}

void EVMAsmPrinter::emitInstruction(const MachineInstr *MI) {
  EVMMCInstLower MCInstLowering(OutContext, *this);
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);

  bool commented = emitEVMComments(MI, OutStreamer->getCommentOS());
  EmitToStreamer(*OutStreamer, TmpInst);
  if (commented) {
    OutStreamer->addBlankLine();
  }
}

void EVMAsmPrinter::emitXXStructorList(const DataLayout &DL, const Constant *List,
                                       bool IsCtor) {
  // TODO:IMPLEMENT!!!
}

void EVMAsmPrinter::emitConstantPool() {
  auto &F = CurrentFunc->getFunction();
  if (F.isIntrinsic())
    return;

  auto Sym = cast<MCSymbolEVM>(getSymbol(&F));
  auto Streamer =
      static_cast<EVMTargetStreamer*>(OutStreamer->getTargetStreamer());
  Streamer->emitType(Sym, "function");

  // Emit function signature only for functions with [[evm]] attribute.
  if (!F.getAttributes().hasFnAttr(Attribute::EvmFunc))
    return;

  const DataLayout &DL(F.getParent()->getDataLayout());
  SmallVector<EVT, 4> EResults;
  SmallVector<EVT, 4> EParams;
  const EVMTargetLowering &TLI =
      *TM.getSubtarget<EVMSubtarget>(F).getTargetLowering();
  auto Signature = Sym->getSignature();

  for (auto *Param : F.getFunctionType()->params())
    ComputeValueVTs(TLI, DL, Param, EParams);
  for (EVT VT : EParams) {
    auto RegisterVT = TLI.getRegisterType(F.getContext(), VT);
    Signature->Params.push_back(valTypeFromMVT(RegisterVT));
  }

  ComputeValueVTs(TLI, DL, F.getFunctionType()->getReturnType(), EResults);
  for (EVT VT : EResults) {
    auto RegisterVT = TLI.getRegisterType(F.getContext(), VT);
    Signature->Returns.push_back(valTypeFromMVT(RegisterVT));
  }
  Streamer->emitFunctionType(Sym);
}

bool EVMAsmPrinter::emitBigInt(const ConstantInt *CI) {
  OutStreamer->emitIntValue(CI->getValue());
  return true;
}

void EVMAsmPrinter::printOperand(const MachineInstr *MI, unsigned OpNo,
                                 raw_ostream &OS) {
  const MachineOperand &MO = MI->getOperand(OpNo);

  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    break;
  case MachineOperand::MO_Immediate:
    OS << MO.getImm();
    break;
  case MachineOperand::MO_CImmediate:
    OS << MO.getCImm();
    break;
  case MachineOperand::MO_MachineBasicBlock:
    OS << *MO.getMBB()->getSymbol();
    break;
  case MachineOperand::MO_BlockAddress: {
    MCSymbol *Sym = GetBlockAddressSymbol(MO.getBlockAddress());
    OS << Sym->getName();
    break;
  }
  default:
    llvm_unreachable("Not implemented yet!");
  }
}

bool EVMAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                    const char *ExtraCode, raw_ostream &OS) {
  printOperand(MI, OpNo, OS);
  return true;
}

bool EVMAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                            unsigned OpNo,
                                            const char *ExtraCode,
                                            raw_ostream &OS) {

  return AsmPrinter::PrintAsmMemoryOperand(MI, OpNo, ExtraCode, OS);
}

// Force static initialization.
extern "C" void LLVMInitializeEVMAsmPrinter() {
  RegisterAsmPrinter<EVMAsmPrinter> Y(getTheEVMTarget());
}
