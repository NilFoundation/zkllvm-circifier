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

#include "evm_linker.h"
#include "nil/crypto3/hash/algorithm/hash.hpp"
#include "nil/crypto3/hash/keccak.hpp"
#include "llvm/Object/Archive.h"
#include "llvm/Object/ArchiveWriter.h"
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
static cl::opt<bool> MakeArchive("make-archive",
                                 cl::desc("Compose input files into an archive,"
                                          " without linking"),
                                 llvm::cl::init(false));
static cl::opt<unsigned> StackAlignment("stack-align",
                                cl::desc("Stack alignment in bytes"),
                                llvm::cl::init(8));

static std::unique_ptr<ToolOutputFile> OpenOutputFile(std::string_view Path);
static void WriteDataToStream(raw_fd_ostream& OS, const void* Data, size_t Size);


int EvmLinker::run() {
  if (MakeArchive) {
    return emitArchive();
  }
  Files.reserve(InputFilenames.size());
  for (auto& InputFile : InputFilenames) {
    // At first, check that the file is an archive file, if so, unpack it and
    // process all its containing files.
    if (auto Buf = MemoryBuffer::getFile(InputFile, false, false); Buf) {
      if (Buf.get()->getBuffer().startswith(object::ArchiveMagic)) {
        Expected<std::unique_ptr<object::Archive>> ArchiveOrError =
            object::Archive::create(Buf.get()->getMemBufferRef());
        if (!ArchiveOrError) {
          std::string Message;
          handleAllErrors(ArchiveOrError.takeError(),
                          [&](ErrorInfoBase &EIB) {
            Message = EIB.message();
          });
          report_fatal_error(Twine("Read archive failed: ") + Message);
        }
        readArchive(std::move(ArchiveOrError.get()), InputFile, Files);
        continue;
      }
    }

    auto File = EvmFile::Create(InputFile, SymManager);
    if (!File) {
      LOG() << "Failed to load input file `" << InputFile << '\n';
      return 1;
    }
    Files.push_back(std::move(File));
  }

  for (auto& Func : functions()) {
    if (Func.GlobalInit) {
      assert(GlobalInitFunc == nullptr &&
             "Only one global init function allowed");
      GlobalInitFunc = &Func;
    }
  }

  buildDispatcher();
  if (!NoCtor) {
    buildConstructor();
  }

  SymMap.clear();
  for (auto& File : Files) {
    for (auto& Func: File->getFunctions()) {
      auto InsRes = SymMap.insert(std::make_pair(Func->Name, Func.get()));
      if (!InsRes.second) {
        report_fatal_error(Twine("Duplicated symbol(function): ") + Func->Name);
      }
    }
    for (auto& G: File->getGlobals()) {
      auto InsRes = SymMap.insert(std::make_pair(G->Name, G.get()));
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

int EvmLinker::emitArchive() {
  static std::vector<NewArchiveMember> Members;

  for (auto Input : InputFilenames) {
    auto MemberOrErr = NewArchiveMember::getFile(Input, true);
    if (!MemberOrErr) {
      report_fatal_error("Cannot open input file");
    }
    Members.push_back(std::move(MemberOrErr.get()));
  }
  Error E = writeArchive(OutputFilename, Members, false,
                         object::Archive::Kind::K_GNU, true, false, nullptr);
  if (E) {
    report_fatal_error("writeArchive failed");
  }

  return 0;
}

void EvmLinker::readArchive(std::unique_ptr<object::Archive> Archive,
                            std::string_view FileName,
                            std::vector<std::unique_ptr<EvmFile>>& Files) {
  Error Err = Error::success();
  for (auto &C : Archive->children(Err)) {
    Expected<StringRef> NameOrErr = C.getName();
    if (!NameOrErr) {
      WithColor::error(errs(), "evm-linker") << NameOrErr.takeError();
      report_fatal_error("getName failed");
    }
    Expected<StringRef> BufOrErr = C.getBuffer();
    if (!BufOrErr) {
      WithColor::error(errs(), "evm-linker") << BufOrErr.takeError();
      report_fatal_error("getBuffer failed");
    }
    auto Buffer = MemoryBuffer::getMemBuffer(BufOrErr.get(), FileName, false);
    auto File = EvmFile::Create(NameOrErr.get(), std::move(Buffer), SymManager);
    if (!File) {
      report_fatal_error(Twine("Failed to load archive item: ") +
                         NameOrErr.get());
    }
    Files.push_back(std::move(File));
  }
  if (Err) {
    report_fatal_error("Iterating over Archive's children failed");
  }
}

static uint32_t getFuncHash(Function* Func) {
  // Calculate function hash from its signature.
  // Example of signature: some_function(uint256,uint32[],bytes10,bytes)
  accumulator_set<hashes::keccak_1600<256>> Acc;
  auto addHash = [&Acc](std::string_view S) {
    hash<hashes::keccak_1600<256>>(S.begin(), S.end(), Acc);
  };
  addHash(Func->getBareName());
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
  return Hash;
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

  if (GlobalInitFunc != nullptr) {
    Dispatcher.push("global_init_func");
    Dispatcher.inst(opcodes::PUSH1, 4);
    Dispatcher.inst(opcodes::PC);
    Dispatcher.inst(opcodes::ADD);
    Dispatcher.inst(opcodes::SWAP1);
    Dispatcher.inst(opcodes::JUMP);
    Dispatcher.inst(opcodes::JUMPDEST);
  }

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

      auto Hash = getFuncHash(Func.get());
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
      bool HasPtrArgs = std::find(
                            Func->Inputs.begin(), Func->Inputs.end(),
                            llvm::EVM::ValType::PTR) != Func->Inputs.end();
      if (HasPtrArgs) {
        // If we have struct in the function signature, we need to copy CALLDATA to the memory
        // and set pointer to right place in this memory.
        Dispatcher.inst(opcodes::CALLDATASIZE);
        Dispatcher.push(0);
        Dispatcher.push("stack_start");
        Dispatcher.inst(opcodes::CALLDATACOPY);

        // Calculate real address of inbound struct.
        // TODO: use Fixup with addend, so we can use only one PUSH instruction here
        Dispatcher.push("stack_start");
        Dispatcher.push(4 + (Func->Inputs.size() - 1) * 32);
        Dispatcher.inst(opcodes::ADD);

        for (int i = Func->Inputs.size() - 2; i >= 0; i--) {
          Dispatcher.push(SizeOfFuncId + i * 32);
          Dispatcher.inst(opcodes::CALLDATALOAD);
        }
        Dispatcher.push("stack_start");
        Dispatcher.inst(opcodes::CALLDATASIZE);
        Dispatcher.inst(opcodes::ADD);

        // Round up to 32-bytes alignment
        Dispatcher.push(0x1f);
        Dispatcher.inst(opcodes::ADD);
        Dispatcher.push(0xffe0);
        Dispatcher.inst(opcodes::AND);

        Dispatcher.push(0x00);
        Dispatcher.inst(opcodes::MSTORE);
      } else {
        // Push arguments in reverse order
        for (int i = Func->Inputs.size() - 1; i >= 0; i--) {
          Dispatcher.inst(opcodes::PUSH1, SizeOfFuncId + i * 32);
          Dispatcher.inst(opcodes::CALLDATALOAD);
        }
      }
      Dispatcher.push(Func->Name);
      Dispatcher.inst(opcodes::JUMP);
      Dispatcher.jump_dest(Func->Name + "#return");
      if (!Func->Outputs.empty()) {
        // TODO: Should read function's abi to determine return size.
        assert(Func->Outputs.size() == 1);
        Dispatcher.inst(opcodes::PUSH1, 0);
        Dispatcher.inst(opcodes::MSTORE);
      }
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
  Constructor.push4("contractsize");
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
  GlobalsSectionSize = alignTo(DataOffset, StackAlignment.getValue());

  for (auto& File : Files) {
    File->resolve(SymMap);
  }

  for (auto &File : Files) {
    File->appendInitData(InitData);
  }

  CodeSize = std::accumulate(Files.begin(), Files.end(), 0,
                             [](auto Acc, const auto& F) {
                               return Acc + F->getCode().size();
                             });

  Dispatcher.resolveFixup("init_data_size", static_cast<uint64_t>(InitData.size()));
  Dispatcher.resolveFixup("init_data_code_offset",
                          static_cast<uint64_t>(CodeSize + Dispatcher.size()));
  Dispatcher.resolveFixup("init_data_mem_offset", InitDataOffset);
  Dispatcher.resolveFixup("stack_start", GlobalsSectionSize);
  if (GlobalInitFunc != nullptr) {
    Dispatcher.resolveFixup("global_init_func", GlobalInitFunc->Offset);
  }
  if (!NoCtor) {
    unsigned ContractSize = CodeSize + InitData.size() + Dispatcher.size();
    Constructor.resolveFixup("contractsize", ContractSize);
  }
}

/// Remove all symbols, that are not reachable from root symbols. Root symbols
/// are the symbols, that have `evm` attribute.
void EvmLinker::removeUnreachable() {
  if (KeepUnused) {
    for (auto &Func : functions()) {
      Func.Reachable = true;
    }
    return;
  }
  DependencyGraph DepGraph(SymMap);
  for (auto& File : Files) {
    File->buildDepGraph(DepGraph);
  }
  for (auto &Func : functions()) {
    if (Func.isLeaf()) {
      DepGraph.markReachable(&Func);
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
    WriteDataToStream(Stream->os(), Dispatcher.data(), static_cast<uint64_t>(Dispatcher.size()));
    for (auto &File : Files) {
      LOG() << "Code for " << File->getName() << ", ";
      WriteDataToStream(Stream->os(), File->getCode().data(),
                        File->getCode().size());
    }
    LOG() << "Init data, ";
    WriteDataToStream(Stream->os(), InitData.data(), static_cast<uint64_t>(InitData.size()));
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
      JsonInfo.attribute("dispatcher.size", static_cast<uint64_t>(Dispatcher.size()));
      JsonInfo.attribute("code.size", CodeSize);
      JsonInfo.attribute("globals.offset", static_cast<uint64_t>(CodeSize + Dispatcher.size()));
      JsonInfo.attribute("globals.size", static_cast<uint64_t>(InitData.size()));
      JsonInfo.attribute("code.file", BinOutputName);
    });

    JsonInfo.attributeObject("functions", [&]() {
      for (auto &Func : functions()) {
        JsonInfo.attributeObject(Func.Name, [&]() {
          JsonInfo.attribute("symbol", Func.Name);
          JsonInfo.attribute("name", Func.DemangledName);
          JsonInfo.attribute("offset", Func.Offset);
          JsonInfo.attribute("size", Func.Size);
          JsonInfo.attribute("hash", getFuncHash(&Func));
        });
      }
      JsonInfo.attributeObject("__dispatcher__", [&]() {
        JsonInfo.attribute("symbol", "__dispatcher__");
        JsonInfo.attribute("name", "DISPATCHER");
        JsonInfo.attribute("offset", 0);
        JsonInfo.attribute("size", static_cast<uint64_t>(Dispatcher.size()));
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
                JsonInfo.value(toHex(G->InitData, true));
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
        if (Relocation.first == "fix4" || !SymMap[Relocation.first]->Reachable) {
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
