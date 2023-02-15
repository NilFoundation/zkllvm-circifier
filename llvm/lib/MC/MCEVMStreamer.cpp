//===- lib/MC/MCEVMStreamer.cpp - EVM Object Output ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file assembles .s files and emits EVM .o object files.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCEVMStreamer.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSectionEVM.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#include <iostream>

#define VERBOSE 0

#define LOG() VERBOSE && std::cerr

MCEVMStreamer::~MCEVMStreamer() = default; // anchor.

void MCEVMStreamer::mergeFragment(MCDataFragment *DF, MCDataFragment *EF) {
  flushPendingLabels(DF, DF->getContents().size());

  for (unsigned I = 0, E = EF->getFixups().size(); I != E; ++I) {
    EF->getFixups()[I].setOffset(EF->getFixups()[I].getOffset() +
                                 DF->getContents().size());
    DF->getFixups().push_back(EF->getFixups()[I]);
  }
  if (DF->getSubtargetInfo() == nullptr && EF->getSubtargetInfo())
    DF->setHasInstructions(*EF->getSubtargetInfo());
  DF->getContents().append(EF->getContents().begin(), EF->getContents().end());
}

void MCEVMStreamer::emitAssemblerFlag(MCAssemblerFlag Flag) {
  // Let the target do whatever target specific stuff it needs to do.
  getAssembler().getBackend().handleAssemblerFlag(Flag);

  // Do any generic stuff we need to do.
  llvm_unreachable("invalid assembler flag!");
}

void MCEVMStreamer::emitLabel(MCSymbol *Symbol, SMLoc Loc) {
#if VERBOSE
  LOG() << "emitLabel: "; Symbol->print(errs(), nullptr); LOG() << '\n';
#endif
  MCObjectStreamer::emitLabel(Symbol, Loc);
  auto Section = static_cast<MCSectionEVM*>(getCurrentSectionOnly());
  if (Section->getKind().isData()) {
    assert(Symbol->isEVM());
    Section->addGlobal(Symbol);
  }
}

void MCEVMStreamer::emitIntValue(uint64_t Value, unsigned Size) {
  LOG() << "emitIntValue: " << Value << ", size=" << Size << '\n';
  auto Section = static_cast<MCSectionEVM*>(getCurrentSectionOnly());
  if (Section->getKind().isData()) {
    Section->addGlobalsData(Value);
    auto& CurGlobal = Section->getGlobals().back();
    CurGlobal.DataCount = Section->getGlobalsDataSize() - CurGlobal.DataIndex;
  }
}

void MCEVMStreamer::emitIntValue(APInt Value) {
  auto Section = static_cast<MCSectionEVM*>(getCurrentSectionOnly());
  if (Section->getKind().isData()) {
    Section->addGlobalsData(Value);
    auto& CurGlobal = Section->getGlobals().back();
    CurGlobal.DataCount = Section->getGlobalsDataSize() - CurGlobal.DataIndex;
    LOG() << Section->getGlobalsDataSize() << " emitAPIntValue\n";
  }
}

void MCEVMStreamer::emitBytes(StringRef Data) {
  auto Section = static_cast<MCSectionEVM*>(getCurrentSectionOnly());
  assert (Section->getKind().isData());
  LOG() << Section->getGlobalsDataSize() << " emitBytes: size=" << Data.size()
        << '\n';
  auto& CurGlobal = Section->getGlobals().back();
  for (auto Ch : Data) {
    Section->addGlobalsData(static_cast<unsigned>(Ch));
  }
  CurGlobal.DataCount = Section->getGlobalsDataSize() - CurGlobal.DataIndex;
}

void MCEVMStreamer::emitValueImpl(const MCExpr *Value, unsigned Size,
                                  SMLoc Loc) {
  auto Section = static_cast<MCSectionEVM*>(getCurrentSectionOnly());
  if (Section->getKind().isData()) {
    switch (Value->getKind()) {
    case MCExpr::Constant:
      emitIntValue(dyn_cast<MCConstantExpr>(Value)->getValue(), Size);
      break;
    case MCExpr::SymbolRef: {
      Section->addGlobalsData(Value);
      auto& CurGlobal = Section->getGlobals().back();
      CurGlobal.DataCount = Section->getGlobalsDataSize() - CurGlobal.DataIndex;
      LOG() << Section->getGlobalsDataSize() << " emit Symbol: "
            << dyn_cast<MCSymbolRefExpr>(Value)->getSymbol().getName().data()
            << ", size=" << Size << '\n';
      break;
    }
    case MCExpr::Binary: {
      Section->addGlobalsData(Value);
      auto& CurGlobal = Section->getGlobals().back();
      CurGlobal.DataCount = Section->getGlobalsDataSize() - CurGlobal.DataIndex;
      LOG() << Section->getGlobalsDataSize() << " emit Binary: size=" << Size
            << '\n';
      break;
    }
    default:
      llvm_unreachable("Unsupported MCExpr kind");
    }
  }
}

void MCEVMStreamer::emitFill(const MCExpr &NumBytes, uint64_t FillValue,
                             SMLoc Loc) {
  assert(NumBytes.getKind() == MCExpr::Constant);
  int64_t NumBytesValue;
  if (!NumBytes.evaluateAsAbsolute(NumBytesValue))
    report_fatal_error("Invalid expression for emitFill");
  if (NumBytesValue == 0)
    return;
  if (NumBytesValue < 32)
    return;
  LOG() << "emitFill: " << FillValue << ", size=" << NumBytesValue << '\n';
  if (NumBytesValue < 32) {
    NumBytesValue = alignTo(NumBytesValue, 32);
  }
  assert((NumBytesValue % 32) == 0 && "Only 32 bytes chunk supported");

  for (int i = 0; i < NumBytesValue; i += 32) {
    emitIntValue(FillValue, 32);
  }
}

void MCEVMStreamer::changeSection(MCSection *Section,
                                  const MCExpr *Subsection) {
  changeSectionImpl(Section, Subsection);
}

void MCEVMStreamer::emitWeakReference(MCSymbol *Alias, const MCSymbol *Symbol) {
  getAssembler().registerSymbol(*Symbol);
  const MCExpr *Value = MCSymbolRefExpr::create(
      Symbol, MCSymbolRefExpr::VK_WEAKREF, getContext());
  Alias->setVariableValue(Value);
}

bool MCEVMStreamer::emitSymbolAttribute(MCSymbol *S, MCSymbolAttr Attribute) {

  return true;
}

void MCEVMStreamer::emitCommonSymbol(MCSymbol *S, uint64_t Size,
                                     unsigned ByteAlignment) {
  llvm_unreachable("Common symbols are not yet implemented for EVM");
}

void MCEVMStreamer::emitELFSize(MCSymbol *Symbol, const MCExpr *Value) {
  auto Section = static_cast<MCSectionEVM*>(getCurrentSectionOnly());
  if (Section->getKind().isData()) {
    auto& CurGlobal = Section->getGlobals().back();
    int64_t Size;
    Value->evaluateAsAbsolute(Size);
    CurGlobal.Size = Size;
    cast<MCSymbolEVM>(Symbol)->setSize(Size);
  }
}

void MCEVMStreamer::emitLocalCommonSymbol(MCSymbol *S, uint64_t Size,
                                          unsigned ByteAlignment) {
  llvm_unreachable("Local common symbols are not yet implemented for EVM");
}

void MCEVMStreamer::emitValueToAlignment(unsigned ByteAlignment, int64_t Value,
                                         unsigned ValueSize,
                                         unsigned MaxBytesToemit) {
  MCObjectStreamer::emitValueToAlignment(ByteAlignment, Value, ValueSize,
                                         MaxBytesToemit);
}

void MCEVMStreamer::emitIdent(StringRef IdentString) {
  // TODO(sbc): Add the ident section once we support mergable strings
  // sections in the object format
}

void MCEVMStreamer::emitInstToFragment(const MCInst &Inst,
                                       const MCSubtargetInfo &STI) {
  this->MCObjectStreamer::emitInstToFragment(Inst, STI);
}

void MCEVMStreamer::emitInstToData(const MCInst &Inst,
                                   const MCSubtargetInfo &STI) {
  MCAssembler &Assembler = getAssembler();
  SmallVector<MCFixup, 4> Fixups;
  SmallString<256> Code;
  raw_svector_ostream VecOS(Code);

  // Append the encoded instruction to the current data fragment (or create a
  // new such fragment if the current fragment is not a data fragment).
  MCDataFragment *DF = getOrCreateDataFragment();

  Assembler.getEmitter().encodeInstruction(Inst, VecOS, Fixups, STI);

  // Add the fixups and data.
  for (unsigned I = 0, E = Fixups.size(); I != E; ++I) {
    Fixups[I].setOffset(Fixups[I].getOffset() + DF->getContents().size());
    DF->getFixups().push_back(Fixups[I]);
  }
  DF->setHasInstructions(STI);
  DF->getContents().append(Code.begin(), Code.end());
}

void MCEVMStreamer::finishImpl() {
  emitFrames(nullptr);

  this->MCObjectStreamer::finishImpl();
}

MCStreamer *llvm::createEVMStreamer(MCContext &Context,
                                    std::unique_ptr<MCAsmBackend> &&MAB,
                                    std::unique_ptr<MCObjectWriter> &&OW,
                                    std::unique_ptr<MCCodeEmitter> &&CE,
                                    bool RelaxAll) {
  MCEVMStreamer *S =
      new MCEVMStreamer(Context, std::move(MAB), std::move(OW), std::move(CE));
  if (RelaxAll)
    S->getAssembler().setRelaxAll(true);
  return S;
}

void MCEVMStreamer::emitThumbFunc(MCSymbol *Func) {
  llvm_unreachable("Generic EVM doesn't support this directive");
}

void MCEVMStreamer::emitSymbolDesc(MCSymbol *Symbol, unsigned DescValue) {
  llvm_unreachable("EVM doesn't support this directive");
}

void MCEVMStreamer::emitZerofill(MCSection *Section, MCSymbol *Symbol,
                                 uint64_t Size, unsigned ByteAlignment,
                                 SMLoc Loc) {
  llvm_unreachable("EVM doesn't support this directive");
}

void MCEVMStreamer::emitTBSSSymbol(MCSection *Section, MCSymbol *Symbol,
                                   uint64_t Size, unsigned ByteAlignment) {
  llvm_unreachable("EVM doesn't support this directive");
}
