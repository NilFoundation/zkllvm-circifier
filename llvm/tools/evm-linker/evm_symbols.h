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
#ifndef LLVM_EVM_SYMBOLS_H
#define LLVM_EVM_SYMBOLS_H


#include "llvm/BinaryFormat/EVM.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <set>
#include "llvm/ADT/APInt.h"

struct EvmSymbol;

using SymbolMap = std::unordered_map<std::string, EvmSymbol*>;

struct Relocation {
  enum class SpaceKind {Code, InitData};
  EvmSymbol* TargetSymbol{nullptr};
  unsigned Addend{0};
  unsigned Offset;
  unsigned Size{0};
  SpaceKind Space;
  bool Dead{false};
};

struct EvmSymbol {
  enum class Kind {Function, Global};

  static constexpr unsigned INVALID_OFFSET = (unsigned)-1;

  virtual ~EvmSymbol() = default;

  virtual Kind getKind() const = 0;

  bool isResolved() const {
    return Offset != INVALID_OFFSET;
  }

  std::string_view getBareName() const {
    std::string_view name = DemangledName;
    return name.substr(0, name.find('('));
  }

  std::string Name;
  std::string DemangledName;
  unsigned Offset{INVALID_OFFSET};
  unsigned Size{0};
  bool Reachable{false};
};

struct EvmSymbolNode {
  EvmSymbol* Symbol{nullptr};
  std::vector<EvmSymbolNode*> Children;
#ifndef NDEBUG
  std::string Name;
#endif
};

/// This class builds dependencies between symbols and provides method to mark
/// unreachable symbols in the graph.
class DependencyGraph {
public:
  DependencyGraph(SymbolMap& SM) : SM(SM) {}

  EvmSymbolNode* getOrCreateNode(const std::string Name) {
    if (auto It = Nodes.find(Name); It != Nodes.end()) {
      return It->second.get();
    }
    auto [It, _] = Nodes.insert({Name, std::make_unique<EvmSymbolNode>()});
    auto Node = It->second.get();
    Node->Symbol = SM[Name];
#ifndef NDEBUG
    Node->Name = Name;
#endif
    return Node;
  }

  void addDependency(const std::string &From, const std::string &To) {
    if (From != To) {
      getOrCreateNode(From)->Children.push_back(getOrCreateNode(To));
    }
  }

  void markReachable(EvmSymbol* Symbol) {
    Symbol->Reachable = true;
    auto Node = Nodes.find(Symbol->Name);
    if (Node == Nodes.end()) {
      return;
    }
    assert(Node != Nodes.end());
    markReachableRec(Node->second.get());
  }

  void markReachableRec(EvmSymbolNode* Node) {
    if (VisitedNodes.find(Node) == VisitedNodes.end()) {
      if (Node->Symbol == nullptr) {
#ifndef NDEBUG
        llvm::report_fatal_error(
            llvm::Twine("[DependencyGraph] Unresolved symbol: ") + Node->Name);
#else
        llvm::report_fatal_error("[DependencyGraph] Unresolved symbol");
#endif
      }
      VisitedNodes.insert(Node);
      Node->Symbol->Reachable = true;
      for (auto ChildNode : Node->Children) {
        markReachableRec(ChildNode);
      }
    }
  }

private:
  std::unordered_map<std::string, std::unique_ptr<EvmSymbolNode>> Nodes;
  std::unordered_set<EvmSymbolNode*> VisitedNodes;
  SymbolMap& SM;
};

struct Function : public EvmSymbol {
  virtual ~Function() = default;

  Kind getKind() const override {
    return Kind::Function;
  }
  std::vector<llvm::EVM::ValType> Inputs;
  std::vector<llvm::EVM::ValType> Outputs;

  /**
   *  If function is a leaf in the dependency tree, i.e. function that can't be
   *  removed.
   */
  bool isLeaf() const {
    return Visible || GlobalInit;
  }

  bool Visible{false};
  bool GlobalInit{false};
};

struct Global : public EvmSymbol {
  virtual ~Global() = default;

  Kind getKind() const override {
    return Kind::Global;
  }
  void addInt(const llvm::APInt& V) {
    for (int i = V.getBitWidth() / CHAR_BIT - 1; i >= 0; i--) {
      InitData.push_back(reinterpret_cast<const uint8_t*>(V.getRawData())[i]);
    }
  }

  std::vector<uint8_t> InitData;
};

class SymbolManager {
public:
  SymbolManager() = default;

  unsigned addFunction(std::string_view Name, std::string_view DemangledName,
                       unsigned Offset, unsigned Size, bool Visible) {
    auto Func = std::make_unique<Function>();
    Func->Name = Name;
    Func->DemangledName = DemangledName;
    Func->Visible = Visible;
    Func->Offset = Offset;
    Func->Size = Size;
    Symbols.push_back(std::move(Func));
    return Symbols.size() - 1;
  }

  Function* getSymbolAsFunction(unsigned Index) {
    return static_cast<Function*>(Symbols[Index].get());
  }
private:
  std::vector<std::unique_ptr<EvmSymbol>> Symbols;
};

inline std::ostream& operator<<(std::ostream& os, const Relocation& rel) {
  os << "Relocation " << (rel.Space == Relocation::SpaceKind::Code ? "[code]: " : "[data]: ");
  os << "target=" << rel.TargetSymbol->Name << ", offset=" << rel.Offset << ", size=" << rel.Size;
  return os;
}

#endif // LLVM_EVM_SYMBOLS_H
