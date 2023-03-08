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

#include "EVM.h"
#include "EVMSubtarget.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/ADT/PostOrderIterator.h"

#define DEBUG_TYPE "evm-stackalloc"

namespace llvm {

class EVMOptimizePush : public MachineFunctionPass {
public:
  static inline char ID = 0;

  EVMOptimizePush() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override {
    return "EVM Optimize Push";
  }

private:
  const EVMInstrInfo *TII{nullptr};
  MachineRegisterInfo *MRI{nullptr};
};

static Register getDefRegister(const MachineInstr &MI) {
  assert(MI.getNumExplicitDefs() == 1 &&
         "Supported only single definition instructions");
  return MI.defs().begin()->getReg();
}

bool EVMOptimizePush::runOnMachineFunction(MachineFunction &MF) {
  MRI = &MF.getRegInfo();
  TII = MF.getSubtarget<EVMSubtarget>().getInstrInfo();

  // Propagate PUSH's immediate to its PUSH users:
  // %2 = PUSH_r 1
  // %4 = PUSH_r %2
  // ===>
  // %2 = PUSH_r 1
  // %4 = PUSH_r 1
  //
  for (auto& MBB : MF) {
    SmallVector<MachineInstr*, 8> InstToRemove;
    for (auto It = MBB.begin(), Next = std::next(It); It != MBB.end(); It = Next, ++Next) {
      auto& MI = *It;
      if (MI.getOpcode() == EVM::PUSH32_r &&
          (MI.getOperand(1).isImm() || MI.getOperand(1).isCImm())) {
        auto Reg = getDefRegister(MI);
        for (auto Use = MRI->use_instr_nodbg_begin(Reg);
             Use != MRI->use_instr_nodbg_end(); ) {
          if (Use->getOpcode() == EVM::PUSH32_r) {
            auto &Op = Use->getOperand(1);
            assert(Op.isReg());
            MRI->removeRegOperandFromUseList(&Op);
            Op = MI.getOperand(1);
            Use = MRI->use_instr_nodbg_begin(Reg);
          } else {
            ++Use;
          }
        }
      }
    }
  }

  // Sink PUSH instructions to its uses:
  // bb.1:
  // %2 = PUSH_r 123
  // ...
  // bb.2:
  // %3 = ADD_r %2, %5
  // ===>
  // bb.1:
  // %2 = PUSH_r 123
  // ...
  // bb.2:
  // %9 = PUSH_r 123
  // %3 = ADD_r %9, %5
  //
  ReversePostOrderTraversal<MachineFunction *> RPO(&MF);
  for (MachineBasicBlock* MBB : RPO) {
    SmallVector<MachineInstr*, 8> InstToRemove;
    for (MachineInstr& MI : *MBB) {
      if (MI.getOpcode() != EVM::PUSH32_r) {
        continue;
      }
      auto Reg = getDefRegister(MI);
      auto MBB = MI.getParent();

      for (auto& Use : MRI->use_nodbg_instructions(Reg)) {
        if (MBB != Use.getParent()) {
          auto NewReg = MRI->createVirtualRegister(&EVM::GPRRegClass);
          auto Inst = BuildMI(*Use.getParent(), Use, Use.getDebugLoc(),
                              TII->get(EVM::PUSH32_r), NewReg);
          Inst->addOperand(MI.getOperand(1));
          for (MachineOperand &MOP : Use.explicit_uses()) {
            if (MOP.isReg() && MOP.getReg() == Reg) {
              MOP.setReg(NewReg);
            }
          }
        }
      }
      if (MRI->use_nodbg_instructions(Reg).empty()) {
        InstToRemove.push_back(&MI);
      }
    }
    for (auto MI : InstToRemove) {
      MBB->remove(MI);
    }
  }

  return true;
}

INITIALIZE_PASS(EVMOptimizePush, "evm-optimize-push",
                "EVM Optimize push", false, false)

FunctionPass *createEVMOptimizePushPass() {
  return new EVMOptimizePush();
}

} // namespace llvm
