#include "AssignerTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;
Target &llvm::getTheAssignerTarget() {
  static Target TheAssignerTarget;
  return TheAssignerTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAssignerTargetInfo() {
  RegisterTarget<Triple::assigner> X(getTheAssignerTarget(), "assigner",
                                     "zkLLVM assigner", "Assigner");
}
