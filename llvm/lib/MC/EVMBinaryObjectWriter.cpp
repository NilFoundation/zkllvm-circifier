//===- lib/MC/EVMBinaryObjectWriter.cpp - EVMBinary File Writer ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements EVMBinary object file writer information.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/BinaryFormat/EVM.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCEVMObjectWriter.h"
#include "llvm/MC/MCSectionEVM.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/LEB128.h"
#include "llvm/Support/StringSaver.h"
#include "llvm/Support/JSON.h"
#include <vector>

#include <sstream>
#include <iomanip>

using namespace llvm;

#define DEBUG_TYPE "mc"

namespace {
class EVMBinaryObjectWriter : public MCObjectWriter {
public:
  EVMBinaryObjectWriter(std::unique_ptr<MCEVMObjectTargetWriter> MOTW,
                        raw_pwrite_stream &OS)
      : W(OS, support::big), TargetObjectWriter(std::move(MOTW)) {}
  void reset() override;

private:
  void recordRelocation(MCAssembler &Asm, const MCAsmLayout &Layout,
                        const MCFragment *Fragment, const MCFixup &Fixup,
                        MCValue Target, uint64_t &FixedValue) override;

  void executePostLayoutBinding(MCAssembler &Asm,
                                const MCAsmLayout &Layout) override;

  uint64_t writeObject(MCAssembler &Asm, const MCAsmLayout &Layout) override;

  support::endian::Writer W;

  std::unique_ptr<MCEVMObjectTargetWriter> TargetObjectWriter;
};

} // end anonymous namespace


// TODO
void EVMBinaryObjectWriter::executePostLayoutBinding(MCAssembler &Asm,
                                                const MCAsmLayout &Layout) {

}

// TODO
void EVMBinaryObjectWriter::recordRelocation(MCAssembler &Asm,
                                        const MCAsmLayout &Layout,
                                        const MCFragment *Fragment,
                                        const MCFixup &Fixup, MCValue Target,
                                        uint64_t &FixedValue) {
  MCAsmBackend &Backend = Asm.getBackend();
  bool IsPCRel = Backend.getFixupKindInfo(Fixup.getKind()).Flags &
                 MCFixupKindInfo::FKF_IsPCRel;
  if (IsPCRel){
    llvm_unreachable("unimplemented");
  }

}

// TODO
void EVMBinaryObjectWriter::reset() {

}

std::string getEVMIntegerTypeName(Type* Type) {
  IntegerType* Ty = cast<IntegerType>(Type);
  auto Width = Ty->getBitWidth();
  if (Ty->getSignBit())
    return "i" + std::to_string(Width);
  return "u" + std::to_string(Width);
}

std::string getEVMTypeName(Argument &Arg) {
  auto Type = Arg.getType();
  std::string Res;
  raw_string_ostream StrOS(Res);
  if (Type->isIntegerTy()) {
    return getEVMIntegerTypeName(Type);
  }
  llvm_unreachable("Unsupported type");
}

uint64_t EVMBinaryObjectWriter::writeObject(MCAssembler &Asm,
                                       const MCAsmLayout &Layout) {
  uint64_t StartOffset = W.OS.tell();
  auto Section = Asm.getContext().getEVMSection();

  json::OStream Json(W.OS, 2);

  Json.objectBegin();

  Json.attributeBegin("functions");
  Json.arrayBegin();
  for (auto& F : Section->functions()) {
    Json.objectBegin();

    auto FuncName = demangle(F.F->getName().str());
    FuncName = FuncName.substr(0, FuncName.find('('));
    Json.attribute("name", FuncName);
    Json.attribute("offset", F.Offset);
    Json.attribute("stateMutability", "");
    Json.attribute("type", "function");

    Json.attributeBegin("inputs");
    Json.arrayBegin();
    for (Argument &Arg : F.F->args()) {
      auto TypeString = getEVMTypeName(Arg);
      Json.object([&](){
        Json.attribute("name", Arg.getName());
        Json.attribute("type", TypeString);
      });
    }
    Json.arrayEnd();
    Json.attributeEnd();

    if (!F.F->getReturnType()->isVoidTy() ) {
      Json.attributeBegin("outputs");
      Json.arrayBegin();
      Json.objectBegin();
      // TODO: support all possible return types
      assert(F.F->getReturnType()->isIntegerTy());
      Json.attribute("name", "");
      Json.attribute("type", "i256");
      Json.objectEnd();
      Json.arrayEnd();
      Json.attributeEnd();
    }

    Json.objectEnd();
  }
  Json.arrayEnd();
  Json.attributeEnd();

  Json.attributeBegin("jump_offsets");
  Json.arrayBegin();
  for (auto offset : Section->fixupOffsets()) {
    Json.value(offset);
  }
  Json.arrayEnd();
  Json.attributeEnd();

  std::stringstream ss;
  ss << std::hex;
  for (const MCFragment &F : *Section) {
    if (F.getKind() == llvm::MCFragment::FT_Data) {
      for (uint8_t Ch : cast<MCDataFragment>(F).getContents()) {
        ss << std::setw(2) << std::setfill('0') << (unsigned)Ch;
      }
    }
  }
  Json.attribute("code", ss.str());

  Json.objectEnd();

  return W.OS.tell() - StartOffset;
}

std::unique_ptr<MCObjectWriter>
llvm::createEVMObjectWriter(std::unique_ptr<MCEVMObjectTargetWriter> MOTW,
                            raw_pwrite_stream &OS) {
  return std::make_unique<EVMBinaryObjectWriter>(std::move(MOTW), OS);
}
