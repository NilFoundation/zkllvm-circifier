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
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
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
}

// TODO
void EVMBinaryObjectWriter::reset() {

}

std::string getEVMIntegerTypeName(Type* Type) {
  IntegerType* Ty = cast<IntegerType>(Type);
  auto Width = Ty->getBitWidth();
  if (Ty->getSignBit())
    return "int" + std::to_string(Width);
  return "uint" + std::to_string(Width);
}

uint64_t EVMBinaryObjectWriter::writeObject(MCAssembler &Asm,
                                            const MCAsmLayout &Layout) {
  uint64_t StartOffset = W.OS.tell();
  auto TextSection = Asm.getContext().getEVMSection(".text");

  json::OStream Json(W.OS, 2);

  Json.objectBegin();

  Json.attributeBegin("functions");
  Json.arrayBegin();
  for (const MCSymbol &S : Asm.symbols()) {
    const MCSymbolEVM *Sym = cast_or_null<MCSymbolEVM>(&S);
    if (!Sym || !Sym->isFunction())
      continue;

    assert(Sym->getSize() != 0 && "Should not be empty functions");

    auto Offset = Sym->getOffset();
    auto Size = Sym->getSize();
    bool Visible = !Sym->isHidden();

    Json.objectBegin();

    auto FuncName = demangle(Sym->getName().str());
    Json.attribute("name", FuncName);
    Json.attribute("symbol", Sym->getName().str());
    Json.attribute("offset", Offset);
    Json.attribute("size", Size);
    Json.attribute("visible", Visible);

    // `_GLOBAL__sub_I_` is a prefix for global init function. It is created
    // by LLVM frontend, see CGDeclCXX.cpp.
    if (StringRef(FuncName).startswith("_GLOBAL__sub_I_")) {
      Json.attribute("init", true);
    }

    if (Visible) {
      Json.attribute("stateMutability", "");
      Json.attribute("type", "function");
      Json.attributeArray("inputs", [&] {
        bool WasPtr = false;
        for (auto Param : Sym->Params) {
          if (Visible && WasPtr) {
            report_fatal_error("Pointer argument must be last argument");
          }
          Json.object([&]() {
            Json.attribute("name", "TBD!");
            if (Param->isIntegerTy()) {
              Json.attribute("type", "int256");
            } else if (Param->isPointerTy()) {
              Json.attribute("type", "ptr");
              WasPtr = true;
            } else {
              Param->print(errs());
              report_fatal_error("Unsupported type in EVM function");
            }
          });
        }
      });
      Json.attributeArray("outputs", [&] {
        for (auto Return : Sym->getSignature()->Returns) {
          auto TypeString = EVM::ValTypeToString(Return);
          Json.object([&]() {
            Json.attribute("name", "");
            Json.attribute("type", TypeString);
          });
        }
      });
    }

    Json.objectEnd();
  }
  Json.arrayEnd();
  Json.attributeEnd();

  /**
   * Format of the data elements:
   * - u<size>:<number> - integer value with width of `size` bytes and `value`
   *   as a decimal number
   * - u<size>:@<symbol> - symbol, where `size` is a size of occupied data,
   *   `symbol` is a name of symbol to be relocated in the linker
   * - fill.<size>:value - fill `size` bytes by `value`
   * - bytes:<data> - blob of data, where `data` is a bytes in hex string
   */
  auto DataSection = Asm.getContext().getEVMSection(".data");
  if (!DataSection->getGlobals().empty()) {
    Json.attributeArray("globals", [&] {
      for (auto Global : DataSection->getGlobals()) {
        Json.objectBegin();
        Json.attribute("name", Global.Symbol->getName());
        Json.attribute("size", Global.Size);
        Json.attributeArray("data", [&] {
          for (auto& Data : Global.Data) {
            std::stringstream SS;
            if (Data.isValue()) {
              auto& Value = Data.getValue();
              SS << 'u' << Value.getBitWidth() / CHAR_BIT << ':';
              SmallString<80> SVal;
              Value.toStringUnsigned(SVal);
              SS << (std::string)SVal;
            } else if (Data.isExpression()) {
              SS << "u8:";
              if (auto SymExpr = dyn_cast<MCSymbolRefExpr>(
                  Data.getExpression())) {
                SS << '@' << SymExpr->getSymbol().getName().str();
              } else if (auto SymExpr = dyn_cast<MCBinaryExpr>(
                             Data.getExpression())) {
                std::string Str;
                raw_string_ostream OS(Str);
                SymExpr->print(OS, nullptr);
                SS << '@' << OS.str();
              }
            } else if (Data.isFill()) {
              auto F = Data.getFill();
              SS << "fill." << F.NumBytes << ':' << F.Value;
            } else if (Data.isBytes()) {
              SS << "bytes:" << toHex(ArrayRef(Data.getBytes()), true);
            } else {
              llvm_unreachable("Unexpected global value kind");
            }
            Json.value(SS.str());
          }
        });
        Json.objectEnd();
      }
    });
  }

  Json.attributeObject("relocations", [&] {
    Json.attributeObject("abs4", [&] {
      for (auto &Rel : TextSection->getRelocations()) {
        Json.attributeArray(Rel.first, [&]{
          for (auto Offset : Rel.second) {
            Json.value(Offset);
          }
        });
      }
    });

    Json.attributeArray("fix4", [&] {
      for (auto F : TextSection->fixups()) {
        assert(F.Size == 4);
        Json.value(F.Offset);
      }
    });
  });


  std::stringstream ss;
  ss << std::hex;
  for (const MCFragment &F : *TextSection) {
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
