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

#include "evm_builder.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/Twine.h"

#include <limits>

using namespace llvm;

namespace evm {

void Builder::push(uint32_t Value) {
  if (Value <= std::numeric_limits<uint8_t>::max()) {
    inst(opcodes::PUSH1, Value);
  } else if (Value <= std::numeric_limits<uint16_t>::max()) {
    inst(opcodes::PUSH2, Value);
  } else if (Value <= 0xffffff) {
    inst(opcodes::PUSH3, Value);
  }
  inst(opcodes::PUSH4, Value);
}

void Builder::resolveFixup(const std::string &Name, unsigned Value) {
  auto &F = FixupTable.at(Name);
  F.ResolvedValue = Value;
  assert(!Data.empty());
  for (auto L : F.Locations) {
    writeValue(L.Offset, Value, L.Size);
  }
}

void Builder::emit() {
  for (auto Inst : getInstructions()) {
    if (Inst.getOpcode() < opcodes::PSEUDO) {
      Data.push_back(Inst.getOpcode());
    }
    if (Inst.hasImmediate()) {
      auto Imm = Inst.getImmediate();
      unsigned push_size = opcodes::getPushSize(Inst.getOpcode());
      for (int i = push_size - 1; i >= 0; --i) {
        Data.push_back((Imm >> (i * 8)) & 0xff);
      }
    } else if (Inst.hasJumpTarget()) {
      if (Inst.getOpcode() == opcodes::JUMPDEST || Inst.getOpcode() == opcodes::LABEL) {
        Inst.getJumpTarget()->ResolvedValue = Data.size() - 1;
      } else {
        auto PushSize = opcodes::getPushSize(Inst.getOpcode());
        Inst.getJumpTarget()->Locations.push_back(
            {static_cast<unsigned>(Data.size()), PushSize});
        for (unsigned i = 0; i < PushSize; i++) {
          Data.push_back(0);
        }
      }
    }
  }

  // We're done with the entry code, now functions' code will be encoded. So, it
  // is time to patch all jump offsets, that were just encoded.
  for (auto &It : getFixups()) {
    auto &F = It.second;
    if (F.ResolvedValue == Fixup::UNRESOLVED) {
      continue;
    }
    for (auto L : F.Locations) {
      writeValue(L.Offset, F.ResolvedValue, L.Size);
    }
  }
}

void Builder::writeValue(unsigned Offset, unsigned Value, unsigned Size) {
  assert(Offset + Size < size());
  switch (Size) {
  case 1:
    llvm::support::endian::write<uint8_t, llvm::support::endianness::big>(
        &Data[Offset], Value);
    break;
  case 2:
    llvm::support::endian::write<uint16_t, llvm::support::endianness::big>(
        &Data[Offset], Value);
    break;
  case 4:
    llvm::support::endian::write<uint32_t, llvm::support::endianness::big>(
        &Data[Offset], Value);
    break;
  default:
    report_fatal_error(Twine("Unsupported write size: ") + std::to_string(Size));
  }
}

void Builder::verify() const {
  assert(!Data.empty());
  for (auto &It : getFixups()) {
    if (It.second.ResolvedValue == Fixup::UNRESOLVED) {
      report_fatal_error(Twine("Fixup wasn't resolved: ") + It.first);
    }
  }
}

}  // namespace evm