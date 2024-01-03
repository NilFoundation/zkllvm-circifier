//===- MCSectionEVM.h - EVM Machine Code Sections ---------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the MCSectionEVM class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCSECTIONEVM_H
#define LLVM_MC_MCSECTIONEVM_H

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSymbolEVM.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <unordered_map>
#include <variant>

namespace llvm {

class MCSymbol;
class Function;
class GlobalVariable;

/// This represents a section of EVM.
/// TODO: Use regular entities instead of all those structs inside the section.
class MCSectionEVM final : public MCSection {

public:
  struct FunctionInfo {
    llvm::Function *F;
    unsigned Offset;
    unsigned Size;
  };

  struct Relocation {
    std::string Name;
    unsigned Offset;
  };

  struct Fixup {
    unsigned Offset;
    unsigned Size;
  };

  struct GlobalData {
    struct Fill {
      unsigned NumBytes{0};
      unsigned Value{0};
    };
    using Bytes = std::vector<uint8_t>;
    GlobalData(StringRef V) : Data(Bytes(V.begin(), V.end())) {}

    template<typename T>
    GlobalData(T V) : Data(V) {}

    bool isValue() const {
      return std::holds_alternative<APInt>(Data);
    }
    bool isExpression() const {
      return std::holds_alternative<const MCExpr*>(Data);
    }
    bool isFill() const {
      return std::holds_alternative<Fill>(Data);
    }
    bool isBytes() const {
      return std::holds_alternative<Bytes>(Data);
    }

    APInt& getValue() {
      return std::get<APInt>(Data);
    }
    const APInt& getValue() const {
      return std::get<APInt>(Data);
    }
    const MCExpr* getExpression() {
      return std::get<const MCExpr*>(Data);
    }
    Fill getFill() {
      return std::get<Fill>(Data);
    }
    const Bytes& getBytes() {
      return std::get<Bytes>(Data);
    }

  private:
    std::variant<APInt, const MCExpr*, Fill, Bytes> Data;
  };

  struct Global {
    MCSymbol* Symbol{nullptr};
    unsigned Size{0};
    std::vector<GlobalData> Data;
  };

public:
  friend class MCContext;
  MCSectionEVM(StringRef Name, SectionKind K, MCSymbol *Begin)
      : MCSection(SV_EVM, Name, K, Begin) {}

  void printSwitchToSection(const MCAsmInfo &MAI, const Triple &T,
                            raw_ostream &OS,
                            const MCExpr *Subsection) const override {}

  bool useCodeAlign() const override {
    return false;
  }

  bool isVirtualSection() const override {
    return false;
  }

  void addFunction(Function *F, unsigned Offset, unsigned Size) {
    Functions.push_back({F, Offset, Size});
  }

  void addFixup(unsigned Offset, unsigned Size) {
    Fixups.push_back({Offset, Size});
  }

  const SmallVector<Fixup>& fixups() {
    return Fixups;
  }

  void addRelocation(const std::string &Name, unsigned Offset) {
    Relocations[Name].push_back(Offset);
  }

  const auto& getRelocations() {
    return Relocations;
  }

  void addGlobal(MCSymbol *Symbol) {
    auto& G = Globals.emplace_back();
    G.Symbol = Symbol;
  }

  auto& getGlobals() {
    return Globals;
  }

  template<typename T>
  void addGlobalsData(T Value) {
    getCurrentGLobal().Data.push_back(Value);
  }

  void emitFill(unsigned NumBytes, unsigned Value) {
    getCurrentGLobal().Data.push_back(GlobalData::Fill{NumBytes, Value});
  }

  void emitBytes(StringRef Data) {
    getCurrentGLobal().Data.push_back(Data);
  }

private:
  Global& getCurrentGLobal() {
    assert(!Globals.empty());
    return Globals.back();
  }

private:
  SmallVector<FunctionInfo> Functions;
  // Fixup offset is an offset of PUSH's immediate value, that represents offset
  // for a jump instruction.
  SmallVector<Fixup> Fixups;
  std::unordered_map<std::string, std::vector<unsigned>> Relocations;
  std::vector<Global> Globals;
};

} // end namespace llvm

#endif
