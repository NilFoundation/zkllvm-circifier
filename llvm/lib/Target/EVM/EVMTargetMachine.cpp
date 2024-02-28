//===-- EVMTargetMachine.cpp - Define TargetMachine for EVM -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about EVM target spec.
//
//===----------------------------------------------------------------------===//

#include "EVMTargetMachine.h"
#include "EVM.h"
#include "EVMTargetObjectFile.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetOptions.h"
using namespace llvm;

#define USE_REGALLOC 0
#define USE_NEW_STACK_ALLOC 1

extern "C" void LLVMInitializeEVMTarget() {
  RegisterTargetMachine<EVMTargetMachine> Y(getTheEVMTarget());
  auto PR = PassRegistry::getPassRegistry();

  initializeEVMConvertRegToStackPass(*PR);
  initializeEVMShrinkpushPass(*PR);
  initializeEVMArgumentMovePass(*PR);
  initializeEVMExpandPseudosPass(*PR);
}

static std::string computeDataLayout(const Triple &TT) {
  return "E-S64-a:0:32-p:64:64-i1:8:8-i8:8:8-i16:16:16-i32"
         ":32:32-i64:32:32-i128:32:32-i256:32:32";
}

static Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                           Optional<Reloc::Model> RM) {
  if (!RM.has_value())
    return Reloc::Static;
  return *RM;
}

EVMTargetMachine::EVMTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   Optional<Reloc::Model> RM,
                                   Optional<CodeModel::Model> CM,
                                   CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(TT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<EVMELFTargetObjectFile>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

namespace {
class EVMPassConfig : public TargetPassConfig {
public:
  EVMPassConfig(EVMTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  EVMTargetMachine &getEVMTargetMachine() const {
    return getTM<EVMTargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
  void addPreRegAlloc() override;
  void addPostRegAlloc() override;

  FunctionPass *createTargetRegisterAllocator(bool) override;

  // No reg alloc
  bool addRegAssignAndRewriteFast() override { return false; }
  // EVM doesn't use reg alloc
  bool addRegAssignAndRewriteOptimized() override {
#if USE_REGALLOC
    return TargetPassConfig::addRegAssignAndRewriteOptimized();
#else
    return false;
#endif
  }
};
} // namespace

TargetPassConfig *EVMTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new EVMPassConfig(*this, PM);
}

void EVMPassConfig::addIRPasses() {
  TargetPassConfig::addIRPasses();
  // addPass(createEVMCallTransformation());
}

bool EVMPassConfig::addInstSelector() {
  TargetPassConfig::addInstSelector();
  addPass(createEVMISelDag(getEVMTargetMachine()));

  // addPass(createEVMArgumentMove());
  return false;
}

void EVMPassConfig::addPreEmitPass() {
  TargetPassConfig::addPreEmitPass();

  // construct stack arguments and move them to the correct location.
  addPass(createEVMArgumentMove());

  // we are going to eliminate $fp and $sp at this point, so subsequent passes
  // only deals with virtual registers.
  addPass(createEVMExpandFramePointer());

  addPass(createEVMOptimizePushPass());

  //  This is the major pass we will use to stackify registers
  addPass(createEVMStackAllocationPass());

  // We use a custom pass to expand pseudos at a later pahse
  addPass(createEVMExpandPseudos());

  // This is the the pass we use to explicitly convert instructions in the
  // reg-based form to stack-based form. Note that this pass neither alters the
  // order nor insert additional instructions.
  addPass(createEVMConvertRegToStack());

  // so far we are only generating PUSH32 instructions, now we will use a
  // shorter version if possible.
  addPass(createEVMShrinkpush());

  // some peepohole at the very end.
  addPass(createEVMFinalization());
}

void EVMPassConfig::addPreRegAlloc() {
  // this will cause a bug:
  // see: https://github.com/juntao/etclabs-secondstate/issues/16
  disablePass(&MachineBlockPlacementID);
  disablePass(&RegisterCoalescerID);
}

void EVMPassConfig::addPostRegAlloc() {
  // copied from WebAssembly backed.

  // These functions all require the NoVRegs property.
  disablePass(&MachineCopyPropagationID);
  disablePass(&PostRAMachineSinkingID);
  disablePass(&PostRASchedulerID);
  disablePass(&FuncletLayoutID);
  disablePass(&StackMapLivenessID);
  disablePass(&LiveDebugValuesID);
  disablePass(&PatchableFunctionID);
  disablePass(&ShrinkWrapID);

  // disable branch folding as it invalidates liveness
  disablePass(&BranchFolderPassID);

  TargetPassConfig::addPostRegAlloc();
}

bool EVMTargetMachine::usesPhysRegsForValues() const {
#if USE_REGALLOC
  return LLVMTargetMachine::usesPhysRegsForValues();
#else
  return false;
#endif
}

// Disable register allocation.
FunctionPass *EVMPassConfig::createTargetRegisterAllocator(bool v) {
#if USE_REGALLOC
  return TargetPassConfig::createTargetRegisterAllocator(v);
#else
  return nullptr; // No reg alloc
#endif
}
