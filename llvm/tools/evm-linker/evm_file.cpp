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

#include "evm_file.h"
#include "evm_opcodes.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Endian.h"
#include <numeric>

using namespace llvm;

static std::string parseRelocSymbol(std::string_view Str, unsigned* Addend);

std::unique_ptr<EvmFile> EvmFile::Create(
    StringRef FileName, SymbolManager& SymManager) {
  ErrorOr<std::unique_ptr<MemoryBuffer>> FileOrErr =
      MemoryBuffer::getFileOrSTDIN(FileName, true);
  if (FileOrErr.getError()) {
    report_fatal_error(Twine("Cannot open file ") + FileName);
  }
  return Create(FileName, std::move(FileOrErr.get()), SymManager);
}

std::unique_ptr<EvmFile> EvmFile::Create(StringRef FileName,
    std::unique_ptr<MemoryBuffer> Buffer, SymbolManager& SymManager) {
  auto File = std::unique_ptr<EvmFile>(new EvmFile);
  File->FileName = FileName;

  auto JsonFile = json::parse(Buffer->getBuffer());
  if (!JsonFile) {
    report_fatal_error(Twine("JSON parse failed, file: ") + FileName);
  }

  auto JsonData = JsonFile->getAsObject();

  // Load functions
  if (auto Functions = JsonData->getArray("functions")) {
    File->Functions.reserve(Functions->size());
    for (auto F : *Functions) {
      auto FuncObj = F.getAsObject();

      auto Func = std::make_unique<Function>();

      Func->Name = FuncObj->getString("symbol")->str();
      Func->DemangledName = FuncObj->getString("name")->str();
      Func->Visible = FuncObj->getBoolean("visible").value();
      Func->Offset = FuncObj->getInteger("offset").value();
      Func->Size = FuncObj->getInteger("size").value();

      auto SymIndex = SymManager.addFunction(Func->Name, Func->DemangledName,
                                             Func->Offset, Func->Size,
                                             Func->Visible);
      auto NewFunc = SymManager.getSymbolAsFunction(SymIndex);

      if (auto InputArray = FuncObj->getArray("inputs")) {
        for (auto V : *InputArray) {
          auto Arg = V.getAsObject();
          auto Type = EVM::parseType(Arg->getString("type").value());
          Func->Inputs.push_back(Type);
          NewFunc->Inputs.push_back(Type);
        }
      }
      if (auto OutputArray = FuncObj->getArray("outputs")) {
        for (auto V : *OutputArray) {
          auto Arg = V.getAsObject();
          auto Type = EVM::parseType(Arg->getString("type").value());
          Func->Outputs.push_back(Type);
          Func->Outputs.push_back(Type);
        }
      }
      File->Functions.push_back(std::move(Func));
    }
  }

  std::sort(File->Functions.begin(), File->Functions.end(),
            [](const auto& lhs, const auto& rhs) {
              return lhs->Offset < rhs->Offset;
            });

  // Load global variables/constants
  if (auto Globals = JsonData->getArray("globals")) {
    for (auto& V : *Globals) {
      auto GObj = V.getAsObject();
      auto G = std::make_unique<Global>();

      G->Name = GObj->getString("name").value();
      G->Size = GObj->getInteger("size").value();
      if (auto Data = GObj->getArray("data")) {
        assert(Data->size() * BytesInEvmWord == G->Size);
        for (auto V : *Data) {
          switch (V.kind()) {
          case json::Value::Number:
            G->InitData.emplace_back(BitsInEvmWord, V.getAsUINT64().value());
            break;
          case json::Value::String: {
            auto Str = V.getAsString().value().str();
            // All symbols are prepended by '@' symbol, otherwise it is a big
            // integer, that will be parsed by APInt.
            if (Str[0] == '@') {
              unsigned Addend;
              auto SymbolName = parseRelocSymbol(Str.substr(1), &Addend);
              auto& Reloc = File->RelocationsMap[std::string(SymbolName)];
              unsigned Offset = static_cast<unsigned>(G->InitData.size());
              Reloc.push_back({G.get(), Addend, Offset, 4,
                               Relocation::SpaceKind::InitData});
              G->InitData.emplace_back(BitsInEvmWord, 0);
            } else {
              APInt Value(BitsInEvmWord, Str, 16);
              G->InitData.push_back(Value);
            }
            break;
          }
          default:
            llvm_unreachable("Unsupported type for globals data");
          }
        }
      }
      File->Globals.push_back(std::move(G));
    }
  }

  // Load code
  auto HexCode = JsonData->getString("code").value();
  // JSON file contains code in a string hex format. Let's convert it to the raw
  // binary format.
  File->Code.resize(HexCode.size() / 2);
  for (unsigned i = 0; i < HexCode.size() / 2; i++) {
    bool Res = to_integer(HexCode.substr(i * 2, 2), File->Code[i], 16);
    if (!Res) {
      report_fatal_error("Function code data is corrupted!");
    }
  }

  if (auto Relocations = JsonData->getObject("relocations")) {
    if (auto fix4 = Relocations->getArray("fix4")) {
      auto& Reloc = File->RelocationsMap["fix4"];
      for (auto Fix : *fix4) {
        unsigned Offset = Fix.getAsUINT64().value();

        // Patch absolute offsets to relative offsets from function's beginning
        auto Func = File->getFunctionByOffset(Offset);
        uint32_t Value = support::endian::read32(&File->Code[Offset],
                                                 support::endianness::big);

        // Make offset relative to its function.
        assert(Value >= Func->Offset);
        Value -= Func->Offset;

        // Fixup can be only in PUSH instructions. Opcode is in `Offset - 1`.
        assert(opcodes::isPush(File->Code[Offset - 1]));

        support::endian::write32(&File->Code[Offset], Value,
                                 support::endianness::big);

        Reloc.push_back({Func, 0, Offset - Func->Offset, 4, Relocation::SpaceKind::Code});
      }
    }
    if (auto abs4 = Relocations->getObject("abs4")) {
      for (auto& [Name, Offsets] : *abs4) {
        auto& Reloc = File->RelocationsMap[Name.str()];
        for (auto V : *Offsets.getAsArray()) {
          unsigned Offset = V.getAsUINT64().value();
          auto Func = File->getFunctionByOffset(Offset);
          Reloc.push_back({Func, 0, Offset - Func->Offset, 4, Relocation::SpaceKind::Code});
        }
      }
    }
  }

  return File;
}

void EvmFile::resolve(const SymbolMap& Map) {
  assert(Offset.has_value());

  for (auto& [Name, Relocations] : RelocationsMap) {
    // Adjust unnamed fixups in the code with 4 bytes size
    if (Name == "fix4") {
      for (auto& Fixup : Relocations) {
        if (!Fixup.TargetSymbol->Reachable) {
          LOG() << "Unreachable: " << Fixup.TargetSymbol->Name << '\n';
          continue;
        }

        assert (Fixup.TargetSymbol->getKind() == EvmSymbol::Kind::Function);
        auto RealOffset = Fixup.Offset + Fixup.TargetSymbol->Offset -
                          this->Offset.value();
        assert(Fixup.Size == 4);
        auto Value = support::endian::read32(&Code[RealOffset],
                                             support::endianness::big);
        LOG() << "Fixup in " << Fixup.TargetSymbol->Name << ": offset=" << RealOffset <<
            ", value=" << Value << '+' << Fixup.TargetSymbol->Offset << '\n';
        Value += Fixup.TargetSymbol->Offset;

        support::endian::write32(&Code[RealOffset], Value,
                                 support::endianness::big);
      }
      continue;
    }
    LOG() <<"Resolve relocations for " <<  Name << '\n';
    auto It = Map.find(Name);
    if (It == Map.end()) {
      report_fatal_error(Twine("Symbol not found: ") + Name, false);
    }

    auto Symbol = It->second;
    if (!Symbol->Reachable) {
      continue;
    }
    if (!Symbol->isResolved()) {
      report_fatal_error(Twine("Symbol was not resolved: ") + Symbol->Name,
                         false);
    }
    for (auto& Reloc : Relocations) {
      if (!Reloc.TargetSymbol->Reachable) {
        continue;
      }
      LOG() << " reloc in symbol: " << Reloc.TargetSymbol->Name << '\n';
      switch (Reloc.Space) {
      case Relocation::SpaceKind::Code: {
        auto Offset = Reloc.Offset +
                      (Reloc.TargetSymbol ? Reloc.TargetSymbol->Offset : 0) -
                      this->Offset.value();
        LOG() << "Resolve in offset=" << Offset << " by value=" << Symbol->Offset << '\n';
        assert(opcodes::getPushSize(Code[Offset - 1]) == 4);
        support::endian::write32(&Code[Offset], Symbol->Offset,
                                 support::endianness::big);
        break;
      }
      case Relocation::SpaceKind::InitData: {
        auto Glob = static_cast<Global*>(Reloc.TargetSymbol);
        Glob->InitData[Reloc.Offset] = Symbol->Offset + Reloc.Addend;
        break;
        }
      }
    }
  }
}

void EvmFile::appendInitData(std::vector<uint8_t>& Data) {
  unsigned Offset = Data.size();
  for (auto& Global : Globals) {
    Data.resize(Data.size() + Global->InitData.size() * BytesInEvmWord);
    for (auto V : Global->InitData) {
      assert(V.getBitWidth() == BitsInEvmWord);
      support::endian::write64be(&Data[Offset + 0], V.getRawData()[3]);
      support::endian::write64be(&Data[Offset + 8], V.getRawData()[2]);
      support::endian::write64be(&Data[Offset + 16], V.getRawData()[1]);
      support::endian::write64be(&Data[Offset + 24], V.getRawData()[0]);
      Offset += BytesInEvmWord;
    }
  }
}

unsigned EvmFile::resolveGlobalsWithInitData(unsigned Offset) {
  for (auto& Global : Globals) {
    if (!Global->InitData.empty() && Global->Reachable) {
      assert(Global->InitData.size() != 0);
      Global->Offset = Offset;
      Offset += Global->InitData.size() * BytesInEvmWord;
    }
  }
  return Offset;
}

unsigned EvmFile::resolveGlobalsWithoutInitData(unsigned Offset) {
  for (auto& Global : Globals) {
    if (Global->InitData.empty() && Global->Reachable) {
      assert(Global->Size != 0);
      Global->Offset = Offset;
      Offset += Global->Size;
    }
  }
  return Offset;
}

void EvmFile::buildDepGraph(DependencyGraph &DepGraph) {
  for (auto& [Name, Relocs] : RelocationsMap) {
    if (Name == "fix4") {
      continue;
    }
    for (auto Reloc : Relocs) {
      if (Reloc.Space == Relocation::SpaceKind::InitData) {
        DepGraph.addDependency(Reloc.TargetSymbol->Name, Name);
        continue;
      }
      auto Func = Reloc.TargetSymbol;
      if (Func == nullptr) {
        report_fatal_error("Cannot find function by offset");
      }
      DepGraph.addDependency(Func->Name, Name);
    }
  }
}

Function* EvmFile::getFunctionByOffset(unsigned Offset) {
  auto FuncIt = std::lower_bound(Functions.begin(),
                                 Functions.end(), Offset,
                                 [](auto& lhs, auto rhs) {
                                     return (lhs->Offset + lhs->Size) < rhs;
                                 });
  if (FuncIt == Functions.end()) {
    auto& Last = Functions.back();
    if (Offset >= Last->Offset && Offset < Last->Offset + Last->Size) {
      return Last.get();
    }
    return nullptr;
  }
  return FuncIt->get();
}

void EvmFile::removeUnreachable() {
  Globals.erase(std::remove_if(Globals.begin(), Globals.end(),
                               [](auto& G) { return !G->Reachable; }),
                Globals.end());
  auto It = std::partition(Functions.begin(), Functions.end(),
                 [](auto& Func) { return Func->Reachable; });
  std::move(It, Functions.end(), std::back_inserter(UnreachableFunctions));
  Functions.erase(It, Functions.end());

  decltype(Code) StrippedCode;
  unsigned Offset = 0;
  for (auto& Func : Functions) {
    if (Func->Reachable) {
      std::copy(&Code[Func->Offset], &Code[Func->Offset + Func->Size],
                std::back_inserter(StrippedCode));
      Func->Offset = Offset;
      Offset += Func->Size;
    }
  }

  Code.swap(StrippedCode);
}

void EvmFile::emitAbi(llvm::json::OStream& OS) {
  for (auto& Func : Functions) {
    if (!Func->Visible) {
      continue;
    }
    OS.object([&] {
      OS.attribute("name", Func->DemangledName);
      OS.attributeArray("inputs", [&] {
        for (auto Ty : Func->Inputs) {
          OS.object([&]{
            // TODO: Get name from input file
            OS.attribute("name", "TODO!");
            OS.attribute("type", EVM::ValTypeToString(Ty));
          });
        }
      });
      OS.attributeArray("outputs", [&] {
        for (auto Ty : Func->Outputs) {
          OS.object([&]{
            OS.attribute("name", "");
            OS.attribute("type", EVM::ValTypeToString(Ty));
          });
        }
      });
    });
  }
}

static std::string parseRelocSymbol(std::string_view Str, unsigned* Addend) {
  auto NPos = Str.find('+');

  if (NPos != Str.npos) {
    std::string SAddend(Str.substr(NPos + 1));
    if (Addend)
      *Addend = std::stoi(SAddend);
    return std::string(Str.substr(0, NPos));
  }
  if (Addend)
    *Addend = 0;
  return std::string(Str);
}
