#include "evm_builder.h"
#include "evm_opcodes.h"
#include "evm_file.h"
#include "llvm/Object/Archive.h"


class EvmLinker {
public:
  class FuncIterator {
  public:
    using ValueType = std::pair<uint32_t, uint32_t>;
    static constexpr uint32_t kInvalidIndex =
        std::numeric_limits<uint32_t>::max();

    FuncIterator() = default;
    FuncIterator(const EvmLinker* linker, ValueType pos)
        : Linker(linker), Pos(pos) {
      nextNonEmptyFile();
    }

    static ValueType End() {
      return {kInvalidIndex, kInvalidIndex};
    }

    void nextNonEmptyFile() {
        Pos.second = 0;
        for (auto File = Linker->Files[Pos.first].get();
             File->getFunctions().empty();
             File = Linker->Files[Pos.first].get()) {
          if (++Pos.first == Linker->Files.size()) {
            Pos = End();
            break;
          }
        }
    }

    FuncIterator& operator++() {
      if (Pos == End()) {
        return *this;
      }
      auto File = Linker->Files[Pos.first].get();
      if (Pos.second == File->getFunctions().size() - 1) {
        if (Pos.first == Linker->Files.size() - 1) {
          Pos = End();
        } else {
          Pos.first++;
          nextNonEmptyFile();
        }
      } else {
        Pos.second++;
      }
      return *this;
    }

    Function& operator*() {
      auto File = Linker->Files[Pos.first].get();
      return *File->getFunctions()[Pos.second].get();
    }

    bool operator!=(const FuncIterator& other) const {
      return Pos != other.Pos;
    }

  private:
    const EvmLinker* Linker{nullptr};
    ValueType Pos{End()};
  };

  class FuncRange {
  public:
    FuncRange(const EvmLinker& linker): Linker(linker) {}

    FuncIterator begin() const {
      if (Linker.Files.empty()) {
        return end();
      }
      return EvmLinker::FuncIterator(&Linker, {0, 0});
    }

    FuncIterator end() const {
      return FuncIterator();
    }
  private:
    const EvmLinker& Linker;
  };

public:
  int run();

  void collectFilesInfo();

  void buildDispatcher();

  void buildConstructor();

  void removeUnreachable();

  void resolve(bool CheckReachable);

  void readArchive(std::unique_ptr<llvm::object::Archive> Archive,
                   std::string_view FileName,
                   std::vector<std::unique_ptr<EvmFile>>& Files);

  void emit();

  int emitArchive();

  FuncRange functions() {
    return FuncRange(*this);
  }

private:
  evm::Builder Dispatcher;
  evm::Builder Constructor;
  std::vector<std::unique_ptr<EvmFile>> Files;
  std::vector<uint8_t> InitData;
  SymbolMap SymMap;
  SymbolManager SymManager;
  unsigned GlobalsSectionSize{0};
  unsigned CodeSize{0};
  Function* GlobalInitFunc{nullptr};
};
