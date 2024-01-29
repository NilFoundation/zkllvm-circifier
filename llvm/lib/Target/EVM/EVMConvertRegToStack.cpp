//===-- EVMConvertRegToStack.cpp - Move virtual regisers to memory location -===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the actual transformation from register based
/// instructions to stack based instruction. It considers the order of the
/// instructions in a basicblock is correct, and it does not change the order.
/// Some stack manipulation instructions are inserted into the code.
///
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/EVMMCTargetDesc.h"
#include "EVM.h"
#include "EVMMachineFunctionInfo.h"
#include "EVMSubtarget.h"
#include "EVMUtils.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "evm-reg-to-stacks"

/// Copied from WebAssembly's backend.
namespace {
class EVMConvertRegToStack final : public MachineFunctionPass {
public:
  static char ID; // Pass identification, replacement for typeid
  EVMConvertRegToStack() : MachineFunctionPass(ID) {}

private:
  StringRef getPassName() const override {
    return "EVM Convert register to stacks";
  }

  const TargetInstrInfo* TII;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  int convertCall(MachineInstr &MI, MachineBasicBlock &MBB) const;
  void convertDUP(MachineInstr* MI) const;
  MachineInstr* convertSWAP(MachineInstr& MI, int Index = -1) const;
};
} // end anonymous namespace

char EVMConvertRegToStack::ID = 0;
INITIALIZE_PASS(EVMConvertRegToStack, DEBUG_TYPE,
                "Convert register-based instructions into stack-based",
                false, false)

FunctionPass *llvm::createEVMConvertRegToStack() {
  return new EVMConvertRegToStack();
}

static unsigned getSWAPOpcode(unsigned idx) {
  switch (idx) {
    case 1 : return EVM::SWAP1;
    case 2 : return EVM::SWAP2;
    case 3 : return EVM::SWAP3;
    case 4 : return EVM::SWAP4;
    case 5 : return EVM::SWAP5;
    case 6 : return EVM::SWAP6;
    case 7 : return EVM::SWAP7;
    case 8 : return EVM::SWAP8;
    case 9 : return EVM::SWAP9;
    case 10: return EVM::SWAP10;
    case 11: return EVM::SWAP11;
    case 12: return EVM::SWAP12;
    case 13: return EVM::SWAP13;
    case 14: return EVM::SWAP14;
    case 15: return EVM::SWAP15;
    case 16: return EVM::SWAP16;
    default:
      return EVM::SWAP;
  }
}

MachineInstr* EVMConvertRegToStack::convertSWAP(MachineInstr& MI, int Index/* = -1*/) const {
  if (Index == -1) {
      assert(MI.getOpcode() == EVM::SWAP_r);
      Index = MI.getOperand(0).getImm();
  }
  unsigned Opcode = getSWAPOpcode(Index);
  if (Opcode == EVM::SWAP) {
      BuildMI(*MI.getParent(), MI, MI.getDebugLoc(), TII->get(EVM::PUSH32)).addImm(Index);
  }
  return BuildMI(*MI.getParent(), MI, MI.getDebugLoc(), TII->get(Opcode));
}

static unsigned getDUPOpcode(unsigned idx) {
  switch (idx) {
    case 1 : return EVM::DUP1;
    case 2 : return EVM::DUP2;
    case 3 : return EVM::DUP3;
    case 4 : return EVM::DUP4;
    case 5 : return EVM::DUP5;
    case 6 : return EVM::DUP6;
    case 7 : return EVM::DUP7;
    case 8 : return EVM::DUP8;
    case 9 : return EVM::DUP9;
    case 10: return EVM::DUP10;
    case 11: return EVM::DUP11;
    case 12: return EVM::DUP12;
    case 13: return EVM::DUP13;
    case 14: return EVM::DUP14;
    case 15: return EVM::DUP15;
    case 16: return EVM::DUP16;
    default:
      return EVM::DUP;
  }
}

void EVMConvertRegToStack::convertDUP(MachineInstr* MI) const {
  unsigned Index = MI->getOperand(0).getImm();

  unsigned Opcode = getDUPOpcode(Index);
  if (Opcode == EVM::DUP) {
      BuildMI(*MI->getParent(), MI, MI->getDebugLoc(), TII->get(EVM::PUSH32)).addImm(Index);
  }
  BuildMI(*MI->getParent(), MI, MI->getDebugLoc(), TII->get(Opcode));
  MI->removeFromParent();
}

static unsigned getSlotMemOpcode(bool IsStore) {
  switch (EVMMachineFunctionInfo::getStackSlotSizeInBytes()) {
  case 4: return IsStore ? EVM::MSTORE32_r : EVM::MLOAD32_r;
  case 8: return IsStore ? EVM::MSTORE64_r : EVM::MLOAD64_r;
  case 32: return IsStore ? EVM::MSTORE_r : EVM::MLOAD_r;
  default:
    report_fatal_error("Unsupported slot size");
  }
}

int EVMConvertRegToStack::convertCall(MachineInstr &MI, MachineBasicBlock &MBB) const {
  int StackOpcode = -1;

  auto& MF = *MBB.getParent();
  EVMMachineFunctionInfo *MFI = MF.getInfo<EVMMachineFunctionInfo>();

  unsigned FrameSize = MFI->getFrameSizeInBytes();
  assert(FrameSize % MFI->getStackSlotSizeInBytes() == 0);

  auto* InsertBefore = &MI;
  auto MakeInst = [&]<class... Imms>(unsigned Opcode, Imms... imms) {
    auto B = BuildMI(*MI.getParent(), *InsertBefore, MI.getDebugLoc(),
                     TII->get(Opcode));
    (B.addImm(imms), ...);
    return B;
  };

  unsigned FpAddress = MF.getSubtarget<EVMSubtarget>().getFramePointer();

  using namespace EVM;
  auto LoadOpc = getSlotMemOpcode(false);
  auto StoreOpc = getSlotMemOpcode(true);

  // Save current FP in the slot after current frame
  MakeInst(PUSH32, FpAddress).addComment(MF, "Call start");
                                // fpaddr
  MakeInst(MLOAD);              // fp
  MakeInst(DUP1);               // fp, fp
  MakeInst(PUSH32, FrameSize);  // fp, fp, frame_size
  MakeInst(ADD);                // fp, fp + frame_size
  MakeInst(StoreOpc);           // -

  // Update FP to the callee frame
  MakeInst(PUSH32, FpAddress);  // fpaddr
  MakeInst(MLOAD);              // fp
  MakeInst(PUSH32, FrameSize + MFI->getStackSlotSizeInBytes());
                                // fp, frame_size
  MakeInst(ADD);                // new_fp
  MakeInst(DUP1);               // new_fp, new_fp
  MakeInst(PUSH32, FpAddress);  // new_fp, new_fp, fpaddr
  MakeInst(MSTORE);             // new_fp

  // Also update SP to the new FP
  unsigned SpAddress = MF.getSubtarget<EVMSubtarget>().getStackPointer();
  MakeInst(PUSH32, SpAddress);  // new_fp, spaddr
  MakeInst(MSTORE);             // -

  if (MF.getSubtarget<EVMSubtarget>().hasSubroutine()) {
      // With subroutine support we do not push return address on to stack
      StackOpcode = EVM::JUMPSUB;
  } else {
      // Here we build the return address, and insert it as the first
      // argument of the function.
      MakeInst(PUSH1, 4);       // 4
      MakeInst(GETPC);          // 4, pc
      MakeInst(ADD);            // pc + 4

      // Swap the callee address with the return address
      unsigned Uses = std::distance(MI.uses().begin(), MI.uses().end());
      convertSWAP(*InsertBefore, Uses)->setAsmPrinterFlag(
          EVM::BuildCommentFlags(EVM::SUBROUTINE_BEGIN, 0));
      StackOpcode = EVM::JUMP;
  }

  MachineBasicBlock::iterator MIT(MI);

  // Insert last Store instruction after the JUMP instruction, so all following
  // instructions will be inserted before this Store.
  InsertBefore = BuildMI(MF, MI.getDebugLoc(), TII->get(MSTORE)).addComment(MF, "Call end");
  MBB.insertAfter(MIT, InsertBefore);

  MakeInst(JUMPDEST).addComment(MF, "Call continuation");
  MakeInst(PUSH32, FpAddress);  // fpaddr
  MakeInst(MLOAD);              // fp
  MakeInst(PUSH32, MFI->getStackSlotSizeInBytes());
                                // fp, slot_size
  MakeInst(SWAP1);              // slot_size, fp
  MakeInst(SUB);                // fp - slot_size
  MakeInst(LoadOpc);            // caller_fp
  MakeInst(PUSH32, FpAddress);  // caller_fp, fpaddr
  // Last instruction is `InsertBefore`, which is a store, that restore
  // caller_fp into FP.

  return StackOpcode;
}

bool EVMConvertRegToStack::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG({
    dbgs() << "********** Convert register to stack **********\n"
           << "********** Function: " << MF.getName() << '\n';
  });

  TII = MF.getSubtarget<EVMSubtarget>().getInstrInfo();

  for (MachineBasicBlock & MBB : MF) {
    for (MachineBasicBlock::instr_iterator I = MBB.instr_begin(), E = MBB.instr_end(); I != E;) {
      MachineInstr &MI = *I++;

      unsigned opc = MI.getOpcode();

      // Convert irregular reg->stack mapping
      if (opc == EVM::PUSH32_r) {
        assert(MI.getNumOperands() == 2 && "PUSH32_r's number of operands must be 2.");
        if (!MI.getOperand(1).isReg()) {
          BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(EVM::PUSH32)).add(MI.getOperand(1));
        } else {
          // TODO: Legacy, lead to failures in some tests. Need to rework.
          // BuildMI(MBB, MI, MI.getDebugLoc(), TII->get(EVM::DUP1));
        }
        MI.eraseFromParent();
        continue;
      }

      if (opc == EVM::pSTACKARG_r) {
        MI.removeOperand(0);
        MI.setDesc(TII->get(EVM::pSTACKARG));
        continue;
      }

      // expand SWAP
      if (opc == EVM::SWAP_r) {
        convertSWAP(MI)->setComment(MI.getComment());
        MI.removeFromParent();
        continue;
      }

      if (opc == EVM::DUP_r) {
        convertDUP(&MI);
        continue;
      }

      // MOVE instruction is simply a copy.
      if (opc == EVM::pMOVE_r) {
        MI.removeFromParent();
        continue;
      }

      for (MachineOperand &MO : reverse(MI.uses())) {
        // register value:
        if (MO.isReg()) {
          assert(!Register::isPhysicalRegister(MO.getReg()) &&
                 "There should be no physical registers at this point.");
        }
      }

      // now def and uses are handled. should convert the instruction.
      {
        auto RegOpcode = MI.getOpcode();
        auto StackOpcode = llvm::EVM::getStackOpcode(RegOpcode);

        // special handling of 
        if (StackOpcode == -1) {
          // special handling for return pseudo, as we will expand
          // it at the finalization pass.
          switch (RegOpcode) {
          case EVM::pRETURNSUB_r:
          case EVM::pRETURNSUBVOID_r:
            if (MF.getSubtarget<EVMSubtarget>().hasSubroutine()) {
              StackOpcode = EVM::JUMPSUB;
            } else {
              StackOpcode = EVM::JUMP;
            }
            break;
          case EVM::pJUMPSUB_r:
          case EVM::pJUMPSUBVOID_r:
            StackOpcode = convertCall(MI, MBB);
            break;
          }
        }
        if (StackOpcode == -1) {
          LLVM_DEBUG(MI.dump());
          report_fatal_error("Failed to convert instruction to stack version.");
        }

        MI.setDesc(TII->get(StackOpcode));

        // Remove register operands.
        for (unsigned i = 0; i < MI.getNumOperands();) {
          auto &MO = MI.getOperand(i);
          assert(
              MO.isReg() &&
              "By design we should only see register operands at this point");
          MI.removeOperand(i);
        }

      }
    }
  }

  return true;
}
