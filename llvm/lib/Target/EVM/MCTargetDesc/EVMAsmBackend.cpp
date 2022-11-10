//===-- EVMAsmBackend.cpp - EVM Assembler Backend -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/EVMMCTargetDesc.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionEVM.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/FileSystem.h"
#include <cassert>
#include <cstdint>

#include "llvm/MC/MCValue.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#define DEBUG_TYPE "evm_asmbackend"

using namespace llvm;

static cl::opt<unsigned> DebugOffset("evm-debug-offset", cl::init(0),
  cl::Hidden, cl::desc("Artifical offset for relocation"));

static cl::opt<std::string>
EVMMetadataFile("evm_md_file", cl::desc("EVM metadata filename."));

class MCGenEVMInfo {
public:

  // When generating EVM Metadata for assembly source files this emits the EVM 
  // sections.
  static void Emit(const MCAssembler &Asm) {
    if (EVMMetadataFile.empty()) {
      // TODO: change it to an appropriate name
      EVMMetadataFile = "EVMMeta.txt";
    }

    std::error_code EC;
    sys::fs::OpenFlags OpenFlags = sys::fs::OF_None;
    auto FDOut =
        std::make_unique<ToolOutputFile>(EVMMetadataFile, EC, OpenFlags);

    if (EC) {
      WithColor::error() << EC.message() << '\n';
      return;
    }

    FDOut->keep();
  }
};

namespace {

class EVMAsmBackend : public MCAsmBackend {
public:
  EVMAsmBackend(support::endianness Endian) : MCAsmBackend(Endian) {}
  ~EVMAsmBackend() override = default;

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  // No instruction requires relaxation
  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    return false;
  }

  unsigned getNumFixupKinds() const override { return 1; }

  bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override {
    return false;
  }

  void relaxInstruction(MCInst &Inst, const MCSubtargetInfo &STI) const override {}

  bool writeNopData(raw_ostream &OS, uint64_t Count, const MCSubtargetInfo *STI) const override;

  void finish(MCAssembler const &Asm, MCAsmLayout &Layout) const override;

private:
  void applyFixupValue(MutableArrayRef<char> &Contents, size_t Offset,
                       uint16_t Value) const;
};

} // end anonymous namespace

bool EVMAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count, const MCSubtargetInfo *STI) const {
  return true;
}

void EVMAsmBackend::applyFixupValue(MutableArrayRef<char> &Contents,
                                    size_t Offset, uint16_t Value) const {
  if (DebugOffset != 0) {
    LLVM_DEBUG(dbgs() << "Artifically adding " << DebugOffset
                      << " to all Fixup relocation.\n";);
  }
  support::endian::write<uint16_t>(&Contents[Offset + DebugOffset],
                                   static_cast<uint16_t>(Value), Endian);
}

void EVMAsmBackend::finish(const MCAssembler &Asm, MCAsmLayout &Layout) const {
  MCGenEVMInfo::Emit(Asm);

  // also fix up hidden variables such as deploy.size
  for (MCAssembler::const_symbol_iterator it = Asm.symbol_begin(),
                                          ie = Asm.symbol_end();
       it != ie; ++it) {
    if (it->getName() == "deploy.size") {
      size_t offset = it->getOffset();

      MCFragment* Frag = it->getFragment();
      if (!Frag) {
        llvm_unreachable("deploy.size should have a section.");
      }
      MCDataFragment *FragWithFixups = dyn_cast<MCDataFragment>(Frag);
      if (!FragWithFixups) {
        llvm_unreachable("deploy.size should be in a data frag.");
      }

      MutableArrayRef<char> Contents = FragWithFixups->getContents();
      applyFixupValue(Contents, offset, Contents.size());

      LLVM_DEBUG({
        dbgs() << "deploy.size fixed to: " << Contents.size() << "./n";
      });
    }
  }
}

void EVMAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                               const MCValue &Target,
                               MutableArrayRef<char> Data, uint64_t Value,
                               bool IsResolved,
                               const MCSubtargetInfo *STI) const {
  assert(Fixup.getKind() == FK_SecRel_2);
  assert(Value <= 0xFFFF);

  auto MetaSection = Asm.getContext().getEVMSection();
  MetaSection->addFixupOffset(Fixup.getOffset());

  applyFixupValue(Data, Fixup.getOffset(), Value);
}

std::unique_ptr<MCObjectTargetWriter>
EVMAsmBackend::createObjectTargetWriter() const {
  //return createEVMELFObjectWriter(0);
  return createEVMBinaryObjectWriter();
}

MCAsmBackend *llvm::createEVMAsmBackend(const Target &T,
                                        const MCSubtargetInfo &STI,
                                        const MCRegisterInfo &MRI,
                                        const MCTargetOptions &) {
  return new EVMAsmBackend(support::big);
}

