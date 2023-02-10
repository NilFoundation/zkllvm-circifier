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

#ifndef LLVM_EVM_INST_H
#define LLVM_EVM_INST_H

#include <cstdint>
#include <variant>

namespace evm {

struct Fixup;

class Inst {
public:
  Inst(unsigned O) : Opcode(O) {}
  template <typename T>
  Inst(unsigned O, T V) : Operand(V), Opcode(O) {}

  unsigned getOpcode() const {
    return Opcode;
  }

  bool hasOperand() const {
    return !std::holds_alternative<std::monostate>(Operand);
  }

  bool hasImmediate() const {
    return std::holds_alternative<int64_t>(Operand);
  }

  int64_t getImmediate() const {
    return std::get<int64_t>(Operand);
  }

  bool hasJumpTarget() const {
    return std::holds_alternative<Fixup *>(Operand);
  }
  Fixup *getJumpTarget() const {
    return std::get<Fixup *>(Operand);
  }

private:
  std::variant<std::monostate, int64_t, Fixup*> Operand;
  unsigned Opcode;
};

}  // namespace evm

#endif // LLVM_EVM_INST_H
