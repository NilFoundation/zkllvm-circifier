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

#include "evm_opcodes.h"
#include "nil/crypto3/hash/algorithm/hash.hpp"
#include "nil/crypto3/hash/keccak.hpp"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"

#include <fstream>
#include <unordered_map>
#include <variant>

using namespace llvm;

static cl::opt<std::string>
    InputFilename(cl::Positional, cl::desc("<input file>"), cl::init("-"));

static cl::opt<std::string> OutputFilename("o", cl::desc("Output filename"),
                                           cl::value_desc("filename"));

static uint32_t getSignatureHash(const std::string &Name) {
  using namespace nil::crypto3;

  accumulator_set<hashes::keccak_1600<256>> acc;
  hash<hashes::keccak_1600<256>>(Name.begin(), Name.end(), acc);
  auto sha3 = accumulators::extract::hash<hashes::keccak_1600<256>>(acc);

  return (sha3[0] << 24) | (sha3[1] << 16) | (sha3[2] << 8) | sha3[3];
}

struct JumpTarget;

struct EVMInst {
  EVMInst(unsigned O) : Opcode(O) {}
  template <typename T> EVMInst(unsigned O, T V) : Opcode(O), Operand(V) {}

  bool hasOperand() const {
    return !std::holds_alternative<std::monostate>(Operand);
  }

  bool hasImmediate() const { return std::holds_alternative<int64_t>(Operand); }

  int64_t getImmediate() const { return std::get<int64_t>(Operand); }

  bool hasJumpTarget() const {
    return std::holds_alternative<JumpTarget *>(Operand);
  }

  JumpTarget *getJumpTarget() const { return std::get<JumpTarget *>(Operand); }

  unsigned Opcode;
  std::variant<std::monostate, int64_t, JumpTarget *> Operand;
};

struct JumpTarget {
  std::vector<unsigned> PatchOffsets;
  unsigned Offset{0};
  bool FunctionTarget{false};
};

class InstBuilder {
public:
  EVMInst &inst(unsigned Opcode) { return Instructions.emplace_back(Opcode); }

  void inst(unsigned Opcode, int64_t Imm) {
    Instructions.emplace_back(Opcode, Imm);
  }

  void inst(unsigned Opcode, const std::string &Target) {
    auto &JT = JumpMap[Target];
    Instructions.emplace_back(Opcode, &JT);
  }

  void push_target(const std::string &Target) { inst(opcodes::PUSH2, Target); }

  void push_func(const std::string &Target, unsigned Offset) {
    auto &JT = JumpMap[Target];
    JT.FunctionTarget = true;
    JT.Offset = Offset;
    inst(opcodes::PUSH2, Target);
  }

  auto &getInstructions() { return Instructions; }
  auto &getJumpMap() { return JumpMap; }

private:
  std::unordered_map<std::string, JumpTarget> JumpMap;
  std::vector<EVMInst> Instructions;
};

static std::unique_ptr<ToolOutputFile> GetOutputStream() {
  std::string Path;
  if (OutputFilename.empty()) {
    Path = InputFilename + ".eb";
    WithColor::note() << "Output file is not specified, writing to " << Path
                      << '\n';
  } else {
    Path = OutputFilename;
  }
  std::error_code EC;
  auto Out =
      std::make_unique<ToolOutputFile>(Path, EC, sys::fs::OpenFlags::OF_None);
  if (EC) {
    WithColor::error() << EC.message() << '\n';
    return nullptr;
  }

  Out->keep();
  return Out;
}

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv, "Link EVM compiled files\n");

  ErrorOr<std::unique_ptr<MemoryBuffer>> FileOrErr =
      MemoryBuffer::getFileOrSTDIN(InputFilename, true);
  auto JsonFile = json::parse(FileOrErr.get()->getBuffer());
  if (!JsonFile) {
    report_fatal_error("Can not open input file");
  }

  auto JsonData = JsonFile->getAsObject();

  InstBuilder Builder;

  Builder.inst(opcodes::PUSH1, 0x80);
  Builder.inst(opcodes::PUSH1, 0x40);
  Builder.inst(opcodes::MSTORE);
  Builder.inst(opcodes::PUSH1, 0x60);
  Builder.inst(opcodes::PUSH1, 0x00);
  Builder.inst(opcodes::MSTORE);
  // Check that there are at least 4 bytes for function's hash
  Builder.inst(opcodes::PUSH1, 0x4);
  Builder.inst(opcodes::CALLDATASIZE);
  Builder.inst(opcodes::LT);
  Builder.push_target("fail");
  Builder.inst(opcodes::JUMPI);

  // Load callee function's hash
  Builder.inst(opcodes::PUSH1, 0);
  Builder.inst(opcodes::CALLDATALOAD);
  Builder.inst(opcodes::PUSH1, 0xe0);
  Builder.inst(opcodes::SHR);

  auto Functions = JsonData->getArray("functions");
  if (!Functions) {
    report_fatal_error("'functions' not found!");
  }
  // Switch table over all functions
  for (auto F : *Functions) {
    auto Func = F.getAsObject();
    auto sha3 = getSignatureHash(Func->getString("name")->str());
    Builder.inst(opcodes::DUP1);
    Builder.inst(opcodes::PUSH4, sha3);
    Builder.inst(opcodes::EQ);
    Builder.push_target(Func->getString("name")->str() + "#trampoline");
    Builder.inst(opcodes::JUMPI);
  }
  Builder.push_target("fail");
  Builder.inst(opcodes::JUMP);

  // We use trampoline for each function invocation, where we copy function's
  // arguments from the calldata memory to the stack. Then jump to the
  // function code.
  for (auto F : *Functions) {
    auto Func = F.getAsObject();
    Builder.inst(opcodes::JUMPDEST, Func->getString("name")->str() + "#trampoline");
    Builder.inst(opcodes::POP);
    Builder.push_target(Func->getString("name")->str() + "#return");
    auto InputArray = Func->getArray("inputs");
    // Push arguments in reverse order
    for (int i = InputArray->size() - 1; i >= 0; i--) {
      auto InputObj = (*InputArray)[i].getAsObject();
      auto Type = InputObj->getString("type")->str();
      // TODO: support all types
      assert(Type == "i256");
      Builder.inst(opcodes::PUSH1, 4 + i * 32);
      Builder.inst(opcodes::CALLDATALOAD);
    }
    Builder.push_func(Func->getString("name")->str(),
                      Func->getInteger("offset").value());
    Builder.inst(opcodes::JUMP);
    Builder.inst(opcodes::JUMPDEST, Func->getString("name")->str() + "#return");
    // TODO: Should read function's abi to determine return size.
    Builder.inst(opcodes::PUSH1, 0);
    Builder.inst(opcodes::MSTORE);
    Builder.inst(opcodes::PUSH1, 0x20);
    Builder.inst(opcodes::PUSH1, 0);
    Builder.inst(opcodes::RETURN);
  }

  // Build failure block
  Builder.inst(opcodes::JUMPDEST, "fail");
  Builder.inst(opcodes::PUSH1, 0);
  Builder.inst(opcodes::DUP1);
  Builder.inst(opcodes::REVERT);

  std::string EntryData;
  raw_string_ostream OS(EntryData);
  for (auto Inst : Builder.getInstructions()) {
    OS.write(Inst.Opcode);
    if (Inst.hasImmediate()) {
      auto Imm = Inst.getImmediate();
      unsigned push_size = opcodes::getPushSize(Inst.Opcode);
      for (int i = push_size - 1; i >= 0; --i) {
        char byte = (uint64_t)(0x00FF) & (Imm >> (i * 8));
        OS.write(byte);
      }
    } else if (Inst.hasJumpTarget()) {
      if (Inst.Opcode == opcodes::JUMPDEST) {
        Inst.getJumpTarget()->Offset = EntryData.size() - 1;
      } else {
        assert(Inst.Opcode == opcodes::PUSH2 &&
               "Only PUSH2 is supported at the moment");
        Inst.getJumpTarget()->PatchOffsets.push_back(EntryData.size());
        OS.write(0);
        OS.write(0);
      }
    }
  }

  // We're done with the entry code, now functions' code will be encoded. So, it
  // is time to patch all jump offsets, that were just encoded.
  for (auto &It : Builder.getJumpMap()) {
    auto &JT = It.second;
    unsigned Offset = JT.Offset;
    if (JT.FunctionTarget) {
      Offset += EntryData.size();
    }
    assert(Offset != 0 && Offset <= 0xffff);
    for (auto PatchOffset : JT.PatchOffsets) {
      EntryData[PatchOffset] = (Offset >> 8) & 0xff;
      EntryData[PatchOffset + 1] = Offset & 0xff;
    }
  }

  // JSON file contains code in a string hex format. Here we're converting it
  // to the raw binary format.
  auto Code = JsonData->getString("code").value();
  std::vector<uint8_t> CodeBuffer(Code.size() / 2);
  for (unsigned i = 0; i < Code.size() / 2; i++) {
    bool Res = to_integer(Code.substr(i * 2, 2), CodeBuffer[i], 16);
    if (!Res) {
      report_fatal_error("EntryData corrupted!");
    }
  }

  // Patch all jump offsets resided in functions' code. We just increase all
  // jump offset by size of function selector code.
  auto JumpOffsets = JsonData->getArray("jump_offsets");
  if (JumpOffsets) {
    for (auto it : *JumpOffsets) {
      auto Offset = it.getAsInteger().value();
      uint16_t JumpTarget = CodeBuffer[Offset] << 8 | CodeBuffer[Offset + 1];
      JumpTarget += EntryData.size();
      CodeBuffer[Offset] = (JumpTarget >> 8) & 0xff;
      CodeBuffer[Offset + 1] = JumpTarget & 0xff;
    }
  }

  // Finally, write all data to the output stream.
  auto OutputStream = GetOutputStream();
  if (!OutputStream) {
    return 1;
  }
  OutputStream->os().write(EntryData.data(), EntryData.size());
  OutputStream->os().write(reinterpret_cast<char*>(CodeBuffer.data()),
                           CodeBuffer.size());

  return 0;
}
