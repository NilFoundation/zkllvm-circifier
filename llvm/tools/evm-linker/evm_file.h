//---------------------------------------------------------------------------//
// Copyright 2022 =nil; Foundation
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//
#ifndef LLVM_EVM_FILE_H
#define LLVM_EVM_FILE_H

#include "evm_common.h"
#include "evm_symbols.h"
#include "llvm/Support/Error.h"
#include "llvm/BinaryFormat/EVM.h"
#include <memory>
#include <optional>
#include <variant>
#include <string>
#include <vector>

#include "llvm/Support/JSON.h"

namespace llvm::json {
class Object;
class OStream;
class Value;
}

class EvmFile {

  EvmFile() = default;

public:

  using SymbolMap = std::unordered_map<std::string, EvmSymbol*> ;

  void resolve(const SymbolMap& Map);

  void setOffset(unsigned O) {
    Offset = O;
  }

  std::string_view getName() const {
    return FileName;
  }

  const auto& getFunctions() const {
    return Functions;
  }

  auto& getFunctions() {
    return Functions;
  }

  const auto& getGlobals() const {
    return Globals;
  }

  auto& getGlobals() {
    return Globals;
  }

  const auto& getRelocations() const {
    return RelocationsMap;
  }

  const auto& getCode() const {
    return Code;
  }

  void buildDepGraph(DependencyGraph &DepGraph);

  void removeUnreachable();

  void addRelocation(std::string_view Name, llvm::json::Value& Data);

  void appendInitData(std::vector<uint8_t>& Data);

  unsigned resolveGlobalsWithInitData(unsigned Offset);

  unsigned resolveGlobalsWithoutInitData(unsigned Offset);

  void emitAbi(llvm::json::OStream& OS);

  static std::unique_ptr<EvmFile> Create(
      const std::string& FileName, SymbolManager& SymManager);

private:
  Function* getFunctionByOffset(unsigned Offset);

private:
  std::string FileName;
  std::optional<unsigned> Offset;
  std::vector<uint8_t> Code;
  std::vector<std::unique_ptr<Function>> Functions;
  std::vector<std::unique_ptr<Function>> UnreachableFunctions;
  std::vector<std::unique_ptr<Global>> Globals;
  std::unordered_map<std::string, std::vector<Relocation>> RelocationsMap;
};

#endif // LLVM_EVM_FILE_H
