#ifndef LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETMACHINE_H
#define LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
class AssignerTargetMachine : public LLVMTargetMachine {
public:
  AssignerTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
                   CodeGenOpt::Level OL, bool JIT);
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_ASSIGNER_ASSIGNERTARGETMACHINE_H
