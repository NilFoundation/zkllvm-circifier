//---------------------------------------------------------------------------//
// Copyright 2022 nil.foundation
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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef LLVM_EVM_BUILDER_H
#define LLVM_EVM_BUILDER_H

#include "evm_inst.h"
#include "evm_opcodes.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace evm {

struct Fixup {
  static constexpr int UNRESOLVED = -1;
  struct Location {
    unsigned Offset;
    uint8_t Size;
  };
  std::vector<Location> Locations;
  int ResolvedValue{UNRESOLVED};
};

class Builder {
public:
  Inst &inst(unsigned Opcode) {
    return Instructions.emplace_back(Opcode);
  }

  void inst(unsigned Opcode, int64_t Imm) {
    Instructions.emplace_back(Opcode, Imm);
  }

  void jump_dest(const std::string &Target) {
    auto &P = FixupTable[Target];
    Instructions.emplace_back(opcodes::JUMPDEST, &P);
  }

  void push(uint32_t Value);

  void push(const std::string &Target) {
    auto &PT = FixupTable[Target];
    Instructions.emplace_back(opcodes::PUSH4, &PT);
  }

#define DEFINE_PUSH(N) \
  void push ## N(const std::string &Target) { \
    auto &PT = FixupTable[Target]; \
    Instructions.emplace_back(opcodes::PUSH ## N, &PT); \
  }
  DEFINE_PUSH(1);
  DEFINE_PUSH(2);
  DEFINE_PUSH(3);
  DEFINE_PUSH(4);
  DEFINE_PUSH(5);
  DEFINE_PUSH(6);
  DEFINE_PUSH(7);
  DEFINE_PUSH(8);

  void label(const std::string &Name) {
    auto &P = FixupTable[Name];
    Instructions.emplace_back(opcodes::LABEL, &P);
  }

  auto &getInstructions() {
    return Instructions;
  }

  const auto &getFixups() const {
    return FixupTable;
  }

  size_t size() const {
    return Data.size();
  }

  uint8_t *data() {
    return Data.data();
  }

  void resolveFixup(const std::string &Name, unsigned Value);

  void emit();

  void verify() const;

  void writeValue(unsigned Offset, unsigned Value, unsigned Size);

private:
  std::unordered_map<std::string, Fixup> FixupTable;
  std::vector<Inst> Instructions;
  std::vector<uint8_t> Data;
};

}  // namespace evm

#endif // LLVM_EVM_BUILDER_H
