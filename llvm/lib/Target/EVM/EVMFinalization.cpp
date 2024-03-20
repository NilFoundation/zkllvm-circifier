//===-- EVMFinalization.cpp - Argument instruction moving ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/EVMMCTargetDesc.h"
#include "EVM.h"
#include "EVMMachineFunctionInfo.h"
#include "EVMSubtarget.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "evm-expand-pseudos"

namespace {
class EVMFinalization final : public MachineFunctionPass {
public:
  static char ID; // Pass identification, replacement for typeid
  EVMFinalization() : MachineFunctionPass(ID) {}

private:
  StringRef getPassName() const override {
    return "EVM Finalization";
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  const TargetInstrInfo* TII;
  const EVMSubtarget* ST;

  bool runOnMachineFunction(MachineFunction &MF) override;
  bool shouldInsertJUMPDEST(MachineBasicBlock &MBB) const;
  bool shouldInsertBEGINSUB(MachineBasicBlock &MBB) const;

  void expandADJFP(MachineInstr *MI) const;
};
} // end anonymous namespace

char EVMFinalization::ID = 0;
INITIALIZE_PASS(EVMFinalization, DEBUG_TYPE,
                "Finalize the codegen",
                false, false)

FunctionPass *llvm::createEVMFinalization() {
  return new EVMFinalization();
}

bool EVMFinalization::shouldInsertJUMPDEST(MachineBasicBlock &MBB) const {
  if (ST->hasSubroutine()) {
    return false;
  }

  if (MBB.empty()) {
    return false;
  }

  // Entry MBB needs a basic block.
  if (&MBB == &MBB.getParent()->front()) {
    return true;
  }

  // for now we will add a JUMPDEST anyway.
  return true;
}

bool EVMFinalization::shouldInsertBEGINSUB(MachineBasicBlock &MBB) const {
  if (!ST->hasSubroutine()) {
    return false;
  }

  return true;
}

void EVMFinalization::expandADJFP(MachineInstr* MI) const {
  MachineBasicBlock* MBB = MI->getParent();
  DebugLoc DL = MI->getDebugLoc();
  unsigned opc = MI->getOpcode() == EVM::pADJFPUP ? EVM::ADD : EVM::SUB;

  // Small optimization: if there is no frame adjustment needed,
  // remove the instruction.
  unsigned index = MI->getOperand(0).getImm();
  if (index == 0) {
    MI->eraseFromParent();
    return;
  }

  // we are going to expand ADJFPUP/DOWN into:
  // PUSH1 freeMem
  // MLOAD
  // PUSH1 offset (index * 32)
  // ADD/SUB (according to UP or DOWN)
  // PUSH1 freeMem
  // MSTORE
  unsigned freeMemPointer = ST->getFramePointer();
  BuildMI(*MBB, MI, DL, TII->get(EVM::PUSH1)).addImm(freeMemPointer);
  BuildMI(*MBB, MI, DL, TII->get(EVM::MLOAD));
  BuildMI(*MBB, MI, DL, TII->get(EVM::PUSH1)).addImm(index * 32);
  BuildMI(*MBB, MI, DL, TII->get(opc));
  BuildMI(*MBB, MI, DL, TII->get(EVM::PUSH1)).addImm(freeMemPointer);
  BuildMI(*MBB, MI, DL, TII->get(EVM::MSTORE));

  MI->eraseFromParent();
}

bool EVMFinalization::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG({
    dbgs() << "********** EVM Finalization **********\n"
           << "********** Function: " << MF.getName() << '\n';
  });

  this->ST = &MF.getSubtarget<EVMSubtarget>();
  this->TII = ST->getInstrInfo();

  bool Changed = false;

  for (MachineBasicBlock & MBB : MF) {
    // Insert JUMPDEST at the beginning of the MBB is necessary

    //// TODO: we force each of those MBB's to have address taken.
    //MBB.setHasAddressTaken();
    MachineBasicBlock::iterator begin = MBB.begin();
    if (shouldInsertJUMPDEST(MBB)) {
      BuildMI(MBB, begin, begin->getDebugLoc(), TII->get(EVM::JUMPDEST));
    } else if (shouldInsertBEGINSUB(MBB)) {
      BuildMI(MBB, begin, begin->getDebugLoc(), TII->get(EVM::BEGINSUB));
    }

    // use iterator since we need to remove pseudo instructions
    for (MachineBasicBlock::iterator I = MBB.begin(), E = MBB.end();
         I != E;) {

      MachineInstr *MI = &*I++; 
      unsigned opcode = MI->getOpcode();

      // Remove pseudo instruction
      if (opcode == EVM::pSTACKARG) {
        MI->eraseFromParent();
        Changed = true;
      }

      // TODO: add stack-version of these 2 pseudos
      if (opcode == EVM::pADJFPUP ||
          opcode == EVM::pADJFPDOWN) {
          expandADJFP(MI);
      }
    }
  }

  auto& MFI = *MF.getInfo<EVMMachineFunctionInfo>();
  if (MFI.hasDynamicAlloc()) {
    // For dynamically sized frame, we need to allocate additional slot in
    // frame, and write frame size there. It will be updated each time dynamic
    // allocation occurs.
    auto I = ++(MF.begin()->instr_begin()); //->getNextNode();
    auto &MBB = *MF.begin();
    auto Offset = MF.getFrameInfo().getObjectOffset(MFI.getDynSizeIndex());
    unsigned FpAddress = MF.getSubtarget<EVMSubtarget>().getFramePointer();

    auto TII = MF.getSubtarget<EVMSubtarget>().getInstrInfo();
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::PUSH4))
        .addImm(FpAddress);
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::MLOAD));     // fp
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::DUP1));      // fp, fp
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::PUSH4))
        .addImm(MFI.getFrameSizeInBytes());                 // fp, fp, frame_size
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::ADD));       // fp, new_fp = fp + frame_size
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::SWAP1));     // new_fp, fp
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::PUSH4))
        .addImm(Offset);                                    // new_fp, fp, dyn_size_offset
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::ADD));       // new_fp, dyn_size_slot
    BuildMI(MBB, *I, DebugLoc(), TII->get(EVM::MSTORE));
  }

  return Changed;
}
