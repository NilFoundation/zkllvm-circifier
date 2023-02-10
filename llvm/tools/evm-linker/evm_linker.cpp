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
#include "evm_opcodes.h"
#include "evm_file.h"
#include "nil/crypto3/hash/algorithm/hash.hpp"
#include "nil/crypto3/hash/keccak.hpp"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"

#include <filesystem>
#include <numeric>

using namespace llvm;
using namespace nil::crypto3;

static cl::list<std::string> InputFilenames(cl::Positional, cl::OneOrMore,
                                            cl::desc("<input file>"));
static cl::opt<std::string> OutputFilename("o", cl::desc("Output filename"),
                                           cl::value_desc("filename"));
static cl::opt<bool> NoCtor("no-ctor",
                            cl::desc("Don't generate contract constructor"),
                            llvm::cl::init(false));
static cl::opt<bool> BinaryOutput("binary-output",
                                  cl::desc("Output is raw binary data"),
                                  llvm::cl::init(false));
static cl::opt<bool> KeepUnused("keep-unused",
                                cl::desc("Keep unused symbols in result file"),
                                llvm::cl::init(false));
static cl::opt<bool> OptVerbose("verbose",
                                cl::desc("Verbose output"),
                                llvm::cl::init(false));

static std::unique_ptr<ToolOutputFile> OpenOutputFile(std::string_view Path);
static void WriteDataToStream(raw_fd_ostream& OS, const void* Data, size_t Size);

class EvmLinker {
public:
  int run();

  void collectFilesInfo();

  void buildDispatcher();

  void buildConstructor();

  void removeUnreachable();

  void resolve(bool CheckReachable);


  void emit();

private:
  evm::Builder Dispatcher;
  evm::Builder Constructor;
  std::vector<std::unique_ptr<EvmFile>> Files;
  std::vector<uint8_t> InitData;
  SymbolMap SymbolMap;
  SymbolManager SymManager;
  unsigned GlobalsSectionSize{0};
  unsigned CodeSize{0};
};

int EvmLinker::run() {
  Files.reserve(InputFilenames.size());
  for (auto& InputFile : InputFilenames) {
    auto File = EvmFile::Create(InputFile, SymManager);
    if (!File) {
      LOG() << "Failed to load input file `" << InputFile << '\n';
      return 1;
    }
    Files.push_back(std::move(File));
  }

  buildDispatcher();
  if (!NoCtor) {
    buildConstructor();
  }

  SymbolMap.clear();
  for (auto& File : Files) {
    for (auto& Func: File->getFunctions()) {
      auto InsRes = SymbolMap.insert(std::make_pair(Func->Name, Func.get()));
      if (!InsRes.second) {
        report_fatal_error(Twine("Duplicated symbol(function): ") + Func->Name);
      }
    }
    for (auto& G: File->getGlobals()) {
      auto InsRes = SymbolMap.insert(std::make_pair(G->Name, G.get()));
      if (!InsRes.second) {
        report_fatal_error(Twine("Duplicated symbol(global): ") + G->Name);
      }
    }
  }

  removeUnreachable();
  resolve(true);
  emit();

  return 0;
}


void EvmLinker::buildDispatcher() {
  Dispatcher.push("stack_start");
  Dispatcher.inst(opcodes::PUSH1, 0x00);
  Dispatcher.inst(opcodes::MSTORE);
  Dispatcher.push("init_data_size");
  Dispatcher.push("init_data_code_offset");
  Dispatcher.push("init_data_mem_offset");
  Dispatcher.inst(opcodes::CODECOPY);
  // Check that there are at least 4 bytes for function's hash
  Dispatcher.inst(opcodes::PUSH1, 0x4);
  Dispatcher.inst(opcodes::CALLDATASIZE);
  Dispatcher.inst(opcodes::LT);
  Dispatcher.push("fail");
  Dispatcher.inst(opcodes::JUMPI);

  // Load callee function's hash
  Dispatcher.inst(opcodes::PUSH1, 0);
  Dispatcher.inst(opcodes::CALLDATALOAD);
  Dispatcher.inst(opcodes::PUSH1, 0xe0);
  Dispatcher.inst(opcodes::SHR);

  // Switch table over all functions
  for (auto& File : Files) {
    for (auto& Func : File->getFunctions()) {
      if (!Func->Visible) {
        continue;
      }

      // Calculate function hash form its signature.
      // Example of signature: some_function(uint256,uint32[],bytes10,bytes)
      accumulator_set<hashes::keccak_1600<256>> Acc;
      auto addHash = [&Acc](const std::string& S) {
        hash<hashes::keccak_1600<256>>(S.begin(), S.end(), Acc);
      };
      addHash(Func->DemangledName);
      addHash("(");
      bool IsFirst = true;
      for (auto Input : Func->Inputs) {
        if (!IsFirst) {
          addHash(",");
        }
        addHash(EVM::ValTypeToString(Input));
        IsFirst = false;
      }
      addHash(")");
      auto sha3 = accumulators::extract::hash<hashes::keccak_1600<256>>(Acc);
      auto Hash = (sha3[0] << 24) | (sha3[1] << 16) | (sha3[2] << 8) | sha3[3];
      Dispatcher.inst(opcodes::DUP1);
      Dispatcher.inst(opcodes::PUSH4, Hash);
      Dispatcher.inst(opcodes::EQ);
      Dispatcher.push(Func->Name + "#trampoline");
      Dispatcher.inst(opcodes::JUMPI);
    }
  }
  Dispatcher.push("fail");
  Dispatcher.inst(opcodes::JUMP);

  // We use trampoline for each function invocation, where we copy function's
  // arguments from the calldata memory to the stack. Then jump to the
  // function code.
  for (auto& File : Files) {
    for (auto &Func : File->getFunctions()) {
      if (!Func->Visible) {
        continue;
      }
      Dispatcher.jump_dest(Func->Name + "#trampoline");
      Dispatcher.inst(opcodes::POP);
      Dispatcher.push(Func->Name + "#return");
      // Push arguments in reverse order
      for (int i = Func->Inputs.size() - 1; i >= 0; i--) {
        Dispatcher.inst(opcodes::PUSH1, 4 + i * 32);
        Dispatcher.inst(opcodes::CALLDATALOAD);
      }
      Dispatcher.push(Func->Name);
      Dispatcher.inst(opcodes::JUMP);
      Dispatcher.jump_dest(Func->Name + "#return");
      // TODO: Should read function's abi to determine return size.
      Dispatcher.inst(opcodes::PUSH1, 0);
      Dispatcher.inst(opcodes::MSTORE);
      Dispatcher.inst(opcodes::PUSH1, 0x20);
      Dispatcher.inst(opcodes::PUSH1, 0);
      Dispatcher.inst(opcodes::RETURN);
    }
  }

  // Build failure block
  Dispatcher.jump_dest("fail");
  Dispatcher.inst(opcodes::PUSH1, 0);
  Dispatcher.inst(opcodes::DUP1);
  Dispatcher.inst(opcodes::REVERT);

  Dispatcher.emit();

}

void EvmLinker::buildConstructor() {
  Constructor.inst(opcodes::PUSH1, 0x80);
  Constructor.inst(opcodes::PUSH1, 0x40);
  Constructor.inst(opcodes::MSTORE);
  Constructor.push4("codesize");
  Constructor.inst(opcodes::DUP1);
  Constructor.push1("codestart");
  Constructor.inst(opcodes::PUSH1, 0x00);
  Constructor.inst(opcodes::CODECOPY);
  Constructor.inst(opcodes::PUSH1, 0x00);
  Constructor.inst(opcodes::RETURN);
  Constructor.label("codestart");

  Constructor.emit();
  Constructor.resolveFixup("codestart", Constructor.size());
}


void EvmLinker::resolve(bool CheckReachable) {
  // Update functions offsets according to other files.
  // Also resolve references to the functions in Dispatcher.
  unsigned FileOffset = Dispatcher.size();
  for (auto& File : Files) {
    File->setOffset(FileOffset);
    for (auto &Func : File->getFunctions()) {
      if (!Func->Reachable) {
        continue;
      }
      Func->Offset += FileOffset;
      if (Func->Visible) {
        Dispatcher.resolveFixup(Func->Name, Func->Offset);
      }
    }
    FileOffset += File->getCode().size();
  }

  unsigned DataOffset = InitDataOffset;
  for (auto &File : Files) {
    DataOffset = File->resolveGlobalsWithInitData(DataOffset);
  }

  for (auto &File : Files) {
    DataOffset = File->resolveGlobalsWithoutInitData(DataOffset);
  }
  GlobalsSectionSize = DataOffset;

  for (auto& File : Files) {
    File->resolve(SymbolMap);
  }

  for (auto &File : Files) {
    File->appendInitData(InitData);
  }

  CodeSize = std::accumulate(Files.begin(), Files.end(), 0,
                             [](auto Acc, const auto& F) {
                               return Acc + F->getCode().size();
                             });

  Dispatcher.resolveFixup("init_data_size", InitData.size());
  Dispatcher.resolveFixup("init_data_code_offset",
                          CodeSize + Dispatcher.size());
  Dispatcher.resolveFixup("init_data_mem_offset", InitDataOffset);
  Dispatcher.resolveFixup("stack_start", GlobalsSectionSize);
  if (!NoCtor) {
    Constructor.resolveFixup("codesize", CodeSize + Dispatcher.size());
  }
}

/// Remove all symbols, that are not reachable from root symbols. Root symbols
/// are the symbols, that have `evm` attribute.
void EvmLinker::removeUnreachable() {
  if (KeepUnused) {
    for (auto& File : Files) {
      for (auto &Func : File->getFunctions()) {
        Func->Reachable = true;
      }
    }
    return;
  }
  DependencyGraph DepGraph(SymbolMap);
  for (auto& File : Files) {
    File->buildDepGraph(DepGraph);
  }
  for (auto& File : Files) {
    for (auto &Func : File->getFunctions()) {
      if (Func->Visible) {
        DepGraph.markReachable(Func.get());
      }
    }
  }
  for (auto& File : Files) {
    File->removeUnreachable();
  }
}

void EvmLinker::emit() {

  std::string BinOutputName;

  if (OutputFilename.empty()) {
    BinOutputName = InputFilenames[0] + ".bin";
    WithColor::note() << "Output file is not specified, writing to "
                      << BinOutputName << '\n';
  } else {
    BinOutputName = OutputFilename;
  }

  { // Emit code
    auto Stream = OpenOutputFile(BinOutputName);
    if (!Stream) {
      report_fatal_error("Can't open output file");
    }

    if (!NoCtor) {
      Constructor.verify();
      WriteDataToStream(Stream->os(), Constructor.data(),
                        Constructor.size());
    }
    Dispatcher.verify();
    LOG() << "Dispatcher, ";
    WriteDataToStream(Stream->os(), Dispatcher.data(), Dispatcher.size());
    for (auto &File : Files) {
      LOG() << "Code for " << File->getName() << ", ";
      WriteDataToStream(Stream->os(), File->getCode().data(),
                        File->getCode().size());
    }
    LOG() << "Init data, ";
    WriteDataToStream(Stream->os(), InitData.data(), InitData.size());
  }

  { // Emit Abi file
    std::string OutputName =
        std::filesystem::path(BinOutputName).replace_extension("abi");
    auto Stream = OpenOutputFile(OutputName);
    json::OStream JsonAbi(Stream->os(), 2);
    JsonAbi.arrayBegin();
    for (auto& File : Files) {
      File->emitAbi(JsonAbi);
    }
    JsonAbi.arrayEnd();
  }

  { // Emit auxiliary Info file
    std::string OutputName =
        std::filesystem::path(BinOutputName).replace_extension("info");
    auto Stream = OpenOutputFile(OutputName);
    json::OStream JsonInfo(Stream->os(), 2);
    JsonInfo.objectBegin();

    JsonInfo.attributeObject("info", [&]() {
      JsonInfo.attribute("dispatcher.size", Dispatcher.size());
      JsonInfo.attribute("code.size", CodeSize);
      JsonInfo.attribute("globals.offset", CodeSize + Dispatcher.size());
      JsonInfo.attribute("globals.size", InitData.size());
      JsonInfo.attribute("code.file", BinOutputName);
    });

    JsonInfo.attributeObject("functions", [&]() {
      for (auto &File : Files) {
        for (auto &Func : File->getFunctions()) {
          JsonInfo.attributeObject(Func->Name, [&]() {
            JsonInfo.attribute("symbol", Func->Name);
            JsonInfo.attribute("name", Func->DemangledName);
            JsonInfo.attribute("offset", Func->Offset);
            JsonInfo.attribute("size", Func->Size);
          });
        }
      }
      JsonInfo.attributeObject("__dispatcher__", [&]() {
        JsonInfo.attribute("symbol", "__dispatcher__");
        JsonInfo.attribute("name", "DISPATCHER");
        JsonInfo.attribute("offset", 0);
        JsonInfo.attribute("size", Dispatcher.size());
      });
    });

    unsigned GlobalsSize = 0;
    JsonInfo.attributeObject("globals", [&]() {
      for (auto &File : Files) {
        for (auto &G : File->getGlobals()) {
          GlobalsSize += G->Size;
          JsonInfo.attributeObject(G->Name, [&]() {
            JsonInfo.attribute("name", G->Name);
            JsonInfo.attribute("offset", G->Offset);
            JsonInfo.attribute("size", G->Size);
            if (!G->InitData.empty()) {
              JsonInfo.attributeArray("data", [&]() {
                for (auto V : G->InitData) {
                  if (V.isSingleWord())
                    JsonInfo.value(V.getZExtValue());
                  else {
                    SmallString<80> Str;
                    V.toStringUnsigned(Str, 16);
                    JsonInfo.value(Str);
                  }
                }
              });
            }
          });
        }
      }
    });

    std::unordered_map<std::string, std::vector<Relocation>>
        CollectedRelocations;

    for (auto &File : Files) {
      for (auto& [Symbol, Relocs]: File->getRelocations()) {
        std::copy(Relocs.begin(), Relocs.end(), std::back_inserter(CollectedRelocations[Symbol]));
      }
    }
    JsonInfo.attributeObject("relocations", [&] {
      for (auto Relocation: CollectedRelocations) {
        if (Relocation.first == "fix4" || !SymbolMap[Relocation.first]->Reachable) {
          continue;
        }
        JsonInfo.attributeArray(Relocation.first, [&]() {
          for (auto &Reloc : Relocation.second) {
            if (!Reloc.TargetSymbol->Reachable) {
              continue;
            }
            if (Reloc.Space == Relocation::SpaceKind::Code) {
              std::string Value = Reloc.TargetSymbol->Name + "+" + std::to_string(Reloc.Offset);
              JsonInfo.value(Value);
            }
          }
        });
      }
    });


    JsonInfo.objectEnd();
  }
}

static std::unique_ptr<ToolOutputFile> OpenOutputFile(std::string_view Path) {
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

static void WriteDataToStream(raw_fd_ostream& OS, const void* Data, size_t Size) {
  if (BinaryOutput) {
    OS.write(reinterpret_cast<const char *>(Data), Size);
  } else {
    auto HexData =
        toHex(ArrayRef(reinterpret_cast<const uint8_t*>(Data), Size), true);
    LOG() << "Size=" << Size << '\n';
    OS.write(reinterpret_cast<char*>(HexData.data()), HexData.size());
  }
}

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv, "Link EVM compiled files\n");
  Logger::Enabled = OptVerbose;
  return EvmLinker().run();
}
