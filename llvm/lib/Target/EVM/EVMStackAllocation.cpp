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

#include "EVMStackAllocation.h"
#include "MCTargetDesc/EVMMCTargetDesc.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/ADT/PostOrderIterator.h"

#include <vector>

#define DEBUG_TYPE "evm-stackalloc"

namespace llvm {

INITIALIZE_PASS(EVMStackAllocation, "evm-stack-allocation",
                "EVM Stack Allocation", false, false)

EVMStackAllocation::EVMStackAllocation() : MachineFunctionPass(ID) {
  initializeEVMStackAllocationPass(*PassRegistry::getPassRegistry());
  OperandsPatternMap.resize(OperandsPatternEnum::SIZE);
#define DEF_HANDLER(NAME, VALUE) \
      OperandsPatternMap[VALUE] = &EVMStackAllocation::handleInst<NAME>;
  LOCATION_PATTERNS_LIST(DEF_HANDLER)
#undef DEF_HANDLER
}

bool EVMStackAllocation::runOnMachineFunction(MachineFunction &F) {
  LLVM_DEBUG(dbgs() << "********** Stack Allocation  **********\n"
                    << "********** Function: " << F.getName() << '\n');

  TII = F.getSubtarget<EVMSubtarget>().getInstrInfo();
  LIS = &getAnalysis<LiveIntervals>();
  MRI = &F.getRegInfo();
  MFI = F.getInfo<EVMMachineFunctionInfo>();
  MF = &F;

  MemorySlotsMap.resize(MRI->getNumVirtRegs());

  handleArguments();

  ReversePostOrderTraversal<MachineFunction *> RPO(MF);
  for (MachineBasicBlock* MBB : RPO) {
    LLVM_DEBUG(errs() << "BB: " << MBB->getName() << '\n');
    for (MachineInstr& MI : *MBB) {
      if (MI.getOpcode() == EVM::pPUTLOCAL_r ||
          MI.getOpcode() == EVM::pSTACKARG_r ||
          MI.getOpcode() == EVM::POP_r) {
        continue;
      }
      LLVM_DEBUG(errs() << "==========================\nInst: ";
                 MI.dump();
                 TheStack.dump());

      CurrentPosition = MI.getIterator();

      // Make full cleanup before terminator instructions
      if (MI.getOpcode() == EVM::RETURN_r ||
          MI.getOpcode() == EVM::pRETURNSUB_r ||
          MI.getOpcode() == EVM::pRETURNSUBVOID_r ||
          MI.getOpcode() == EVM::pJUMPIF_r ||
          MI.getOpcode() == EVM::pJUMPTO_r) {
        cleanupStack(MI, true);
      }

      handleOperands(MI);
      executeInstruction(MI);
      // Make soft cleanup after each instruction
      cleanupStack(MI, false);
    }
    if (!TheStack.empty()) {
      TheStack.dump();
      report_fatal_error("Stack must be empty between blocks!");
    }
  }

  MFI->updateMemoryFrameSize(AllocatedSlotNumber);

  return true;
}

void EVMStackAllocation::handleArguments() {
  assert(TheStack.empty());
  SmallVector<MachineInstr*, 4> StackArgs;
  auto MBB = MF->begin();
  for (MachineBasicBlock::iterator It = MBB->begin(), E = MBB->end();
       It != E; ++It) {
    if (It->getOpcode() == EVM::pSTACKARG_r) {
      LLVM_DEBUG(errs() << "Process argument: "; It->dump(););
      auto Reg = getDefRegister(*It);
      auto NumUsers = getUsersCountInSameBB(Reg, *It);
      auto NumUsersOutside = hasUsersInOtherBB(*It);
      if (NumUsersOutside != 0) {
        StackArgs.push_back(&*It);
      }
      TheStack.push(Reg, NumUsers);
      if (NumUsers == 0 && NumUsersOutside == 0) {
        StackArgs.push_back(&*It);
      }
    } else {
      break;
    }
    // All stack manipulations for arguments will be inserted after last arg
    // instruction.
    CurrentPosition = &*It;
  }

  // Walking in reverse order we avoid redundant swap instructions when removing
  // sequential unused args.
  for (auto SAI : reverse(StackArgs)) {
    LLVM_DEBUG(errs() << "Spill argument: "; SAI->dump(););
    auto Reg = getDefRegister(*SAI);
    auto NumUsersOutside = hasUsersInOtherBB(*SAI);
    if (NumUsersOutside != 0) {
      insertSpill(*SAI);
    } else {
      insertSwap<AFTER>(*SAI, Reg);
      insertPop<AFTER>(*SAI);
    }
  }
}

void EVMStackAllocation::handleOperands(MachineInstr &MI) {
  unsigned NumOperands = 0;
  for (const MachineOperand &MOP : MI.explicit_uses()) {
    if (MOP.isReg()) {
      NumOperands++;
    }
  }

  if (NumOperands == 0) {
    return;
  }

  if (NumOperands > 2) {
    LLVM_DEBUG(errs() << "Multiple arguments pattern");
    SeenRegisters.clear();
    SmallVector<unsigned, 8> Indexes;
    Indexes.resize(NumOperands);
    int i = 0;
    for (const MachineOperand &MOP : MI.explicit_uses()) {
      if (MOP.isReg()) {
        Indexes[i] = TheStack.find(MOP.getReg());
        i++;
      }
    }
    unsigned Depth = 0;
    i = NumOperands - 1;
    for (const MachineOperand &MOP : reverse(MI.explicit_uses())) {
      if (!MOP.isReg()) {
        continue;
      }
      auto Reg = MOP.getReg();
      auto Index = Indexes[i];
      bool IsSeenReg = SeenRegisters.find(Register::virtReg2Index(Reg)) !=
                       SeenRegisters.end();
      SeenRegisters[Register::virtReg2Index(Reg)] = true;
      if (Stack::isValidIndex(Index)) {
        // Don't consume stack element if IsSeenReg is true. Register was
        // already consumed in other operand.
        insertDup<BEFORE>(MI, Index + Depth, !IsSeenReg);
      } else {
        insertFill(MI, Reg);
        TheStack.push(Reg, 1);
      }
      Depth++;
      i--;
    }
    return;
  }

  unsigned Pattern = 0;
  auto It = MI.explicit_uses().begin();

  auto Reg0 = It->getReg();
  int Index = TheStack.find(Reg0);

  unsigned NumUsers;
  if (Stack::isValidIndex(Index)) {
    NumUsers = TheStack[Index].getNumUsers();
    Pattern |= OP0_IN_STACK;
  } else {
    NumUsers = getUsersCountInSameBB(Reg0, MI) + 1;
  }
  Pattern |= (NumUsers == 1) ? OP0_ONE_USE : 0;

  Register Reg1;
  if (++It != MI.explicit_uses().end() && It->isReg()) {
    Reg1 = It->getReg();
    Index = TheStack.find(Reg1);
    Pattern |= OP1_EXIST;
    if (Stack::isValidIndex(Index)) {
      NumUsers = TheStack[Index].getNumUsers();
      Pattern |= OP1_IN_STACK;
    } else {
      NumUsers = getUsersCountInSameBB(Reg1, MI) + 1;
    }
    Pattern |= (NumUsers == 1) ? OP1_ONE_USE : 0;
  }
  LLVM_DEBUG(errs() << "Pattern 0x" << utohexstr(Pattern) << ": "
         << getPattternName(Pattern) << '\n');
  assert(OperandsPatternMap[Pattern] != nullptr);
  (this->*OperandsPatternMap[Pattern])(MI, Reg0, Reg1);
}

void EVMStackAllocation::executeInstruction(MachineInstr &MI) {
  LLVM_DEBUG(TheStack.dump());
  for (const MachineOperand &MOP : MI.explicit_uses()) {
    if (!MOP.isReg()) {
      continue;
    }
    assert(!TheStack.empty() && "Stack underflow");
    auto& Item = TheStack.top();
    Item.consume();
    assert(Item.isDead() && "Wrong number of uses");
    TheStack.pop();
  }

  if (MI.getNumExplicitDefs() != 0) {
    auto Reg = getDefRegister(MI);
    auto NumUsers = getUsersCountInSameBB(Reg, MI);
    TheStack.push(Reg, NumUsers);
    if (hasUsersInOtherBB(MI)) {
      insertSpill(MI);
    }

  }
}

unsigned EVMStackAllocation::getUsersCountInSameBB(
    unsigned reg, const MachineInstr &MI) const {
  SlotIndex FstUse = LIS->getInstructionIndex(MI);
  SlotIndex EndOfMBBSI = LIS->getMBBEndIdx(MI.getParent());
  unsigned UsesCount = 0;

  // Iterate over uses and see if any use exists in the same BB.
  for (auto& Use : MRI->use_nodbg_instructions(reg)) {
    if (Use.getOpcode() == EVM::pPUTLOCAL_r) {
      continue;
    }
    SlotIndex SI = LIS->getInstructionIndex(Use);

    // Check if SI lies in between FstUSE and EndOfMBBSI
    if (SlotIndex::isEarlierInstr(FstUse, SI) &&
        SlotIndex::isEarlierInstr(SI, EndOfMBBSI)) {
      UsesCount++;
    }
  }

  return UsesCount;
}

bool EVMStackAllocation::hasUsersInOtherBB(const MachineInstr &MI) const {
  auto Reg = getDefRegister(MI);
  auto MBB = MI.getParent();
  for (auto& Use : MRI->use_nodbg_instructions(Reg)) {
    if (Use.getParent() != MBB) {
      return true;
    }
  }
  return false;
}

/**
 * Remove dead elements from stack. If DeepClean is true, remove all elements,
 * otherwise remove all elements from top to first non-dead element.
 */
void EVMStackAllocation::cleanupStack(MachineInstr &MI, bool DeepClean) {
  if (TheStack.empty()) {
    return;
  }
  if (DeepClean) {
    for (unsigned i = 0; i < TheStack.size();) {
      auto& Item = TheStack[i];
      if (Item.isDead()) {
        insertSwap<BEFORE>(MI, i);
        insertPop<BEFORE>(MI);
      } else {
        i++;
      }
    }
  } else {
    for (auto &Item = TheStack.top(); Item.isDead(); Item = TheStack.top()) {
      insertPop<AFTER>(MI);
      if (TheStack.empty()) {
        break;
      }
    }
  }
}

void EVMStackAllocation::insertSpill(MachineInstr &MI) {
  auto Reg = getDefRegister(MI);
  auto& Slot = MemorySlotsMap[Reg];
  if (Slot == INVALID_SLOT) {
    Slot = AllocatedSlotNumber++;
  }
  LLVM_DEBUG(errs() << "Spill: " << Slot << ": "; MI.dump());
  auto Index = TheStack.find(Reg);
  assert(Stack::isValidIndex(Index));

  TheStack[Index].addUsers(1);
  if (TheStack[Index].getNumUsers() == 1) {
    if (auto Inst = insertSwap<AFTER>(MI, Index)) {
      addInstComment(*Inst, *MF, std::string("SPILL ") + std::to_string(Slot));
    }
  } else {
    if (auto Inst = insertDup<AFTER>(MI, Index)) {
      addInstComment(*Inst, *MF, std::string("SPILL ") + std::to_string(Slot));
    }
  }

  auto PutLocal = BuildMI(*MF, MI.getDebugLoc(), TII->get(EVM::pPUTLOCAL_r))
                      .addReg(Reg)
                      .addImm(Slot);
  addInstComment(*PutLocal.getInstr(), *MF,
                 "PUTLOCAL: reg=",
                 Register::virtReg2Index(Reg),
                 ", slot=", Slot);
  insertAfterCurrent(*PutLocal);
  LIS->InsertMachineInstrInMaps(*PutLocal);
  MemorySlotsMap[Reg] = Slot;
  TheStack.pop();
}

Register EVMStackAllocation::insertFill(MachineInstr &MI, Register Reg) {
  auto Slot = MemorySlotsMap[Reg];
  assert(Slot != INVALID_SLOT && "No slot has been acquired for the register");
  LLVM_DEBUG(errs() << "Fill: slot=" << Slot << ", reg="
                    << Register::virtReg2Index(Reg) << ": "; MI.dump());
  auto DL = MI.getDebugLoc();
  auto GetLocal = BuildMI(*MI.getParent(), MI, DL, TII->get(EVM::pGETLOCAL_r),
                          Reg).addImm(Slot);
  addInstComment(*GetLocal.getInstr(), *MF,
                 "GETLOCAL: reg=",
                 Register::virtReg2Index(Reg),
                 ", slot=", Slot);
  LIS->InsertMachineInstrInMaps(*GetLocal);
  return Reg;
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::U_1U_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  insertSwap<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::U_MU_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  insertDup<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::U_1U_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto TmpReg = insertFill(MI, Reg0);
  TheStack.push(TmpReg, 1);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::U_MU_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto NumUsers = getUsersCountInSameBB(Reg0, MI);
  auto TmpReg = insertFill(MI, Reg0);
  TheStack.push(TmpReg, NumUsers + 1);
  insertDup<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_1U_IS_1U_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto Index0 = TheStack.find(Reg0);
  auto Index1 = TheStack.find(Reg1);
  if (Index0 == Index1) {
    if (Index0 == 0) {
      TheStack[Index0].addUsers(1);
      insertDup<BEFORE>(MI, 0);
    } else {
      TheStack[Index0].addUsers(1);
      insertSwap<BEFORE>(MI, Index0);
      insertDup<BEFORE>(MI, 0);
    }
  } else if (Index0 == 0 && Index1 == 1) {
    // Do nothing - operands are already in their places
  } else if (Index0 == 0 && Index1 != 1) {
    insertSwap<BEFORE>(MI, 1);
    insertSwap<BEFORE>(MI, Index1);
    insertSwap<BEFORE>(MI, 1);
  } else if (Index0 != 0 && Index1 == 1) {
    insertSwap<BEFORE>(MI, Index0);
  } else {
    if (Index0 == 1 && Index1 == 0) {
      insertSwap<BEFORE>(MI, 1);
    } else {
      insertSwap<BEFORE>(MI, Index1);
      insertSwap<BEFORE>(MI, 1);
      if (Index0 != 1) {
        insertSwap<BEFORE>(MI, Index0);
      }
    }
  }
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_1U_IS_MU_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto Index0 = TheStack.find(Reg0);
  auto Index1 = TheStack.find(Reg1);
  if (Index0 == 0) {
    insertDup<BEFORE>(MI, Index1);
    insertSwap<BEFORE>(MI, 1);
  } else {
    insertDup<BEFORE>(MI, Index1);
    insertSwap<BEFORE>(MI, 1);
    insertSwap<BEFORE>(MI, Reg0);
  }
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_1U_IS_1U_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto TmpReg = insertFill(MI, Reg1);
  TheStack.push(TmpReg, 1);

  insertSwap<BEFORE>(MI, 1);
  insertSwap<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_1U_IS_MU_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto NumUsers = getUsersCountInSameBB(Reg1, MI);
  insertFill(MI, Reg1);
  TheStack.push(Reg1, NumUsers + 1);

  insertDup<BEFORE>(MI, 0);
  insertSwap<BEFORE>(MI, 1);
  insertSwap<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_MU_IS_1U_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  insertSwap<BEFORE>(MI, Reg1);
  insertDup<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_MU_IS_MU_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  if (Reg0 == Reg1) {
    auto Index = TheStack.find(Reg0);
    insertDup<BEFORE>(MI, Index);
    insertDup<BEFORE>(MI, Index + 1);
    // Since we count uses in instructions, we need to restore one after double
    // consumption by two DUP instructions.
    TheStack[Index + 2].addUsers(1);
  } else {
    insertDup<BEFORE>(MI, Reg1);
    insertDup<BEFORE>(MI, Reg0);
  }
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_MU_IS_1U_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto TmpReg = insertFill(MI, Reg1);
  TheStack.push(TmpReg, 1);

  insertDup<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_MU_IS_MU_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto NumUsers = getUsersCountInSameBB(Reg1, MI);
  auto TmpReg = insertFill(MI, Reg1);
  TheStack.push(TmpReg, NumUsers + 1);
  insertDup<BEFORE>(MI, 0);

  insertDup<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_1U_IM_1U_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  insertSwap<BEFORE>(MI, Reg1);

  auto TmpReg = insertFill(MI, Reg0);
  TheStack.push(TmpReg, 1);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_1U_IM_MU_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  insertDup<BEFORE>(MI, Reg1);

  auto TmpReg = insertFill(MI, Reg0);
  TheStack.push(TmpReg, 1);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_1U_IM_1U_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto TmpReg = insertFill(MI, Reg1);
  TheStack.push(TmpReg, 1);

  TmpReg = insertFill(MI, Reg0);
  TheStack.push(TmpReg, 1);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_1U_IM_MU_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto NumUsers = getUsersCountInSameBB(Reg1, MI);
  auto TmpReg = insertFill(MI, Reg1);
  TheStack.push(TmpReg, NumUsers + 1);
  insertDup<BEFORE>(MI, 0);

  TmpReg = insertFill(MI, Reg0);
  TheStack.push(TmpReg, 1);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_MU_IM_1U_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto NumUsers = getUsersCountInSameBB(Reg0, MI);
  insertFill(MI, Reg0);
  TheStack.push(Reg0, NumUsers + 1);

  insertSwap<BEFORE>(MI, Reg1);

  insertDup<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_MU_IM_MU_IS>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  insertDup<BEFORE>(MI, Reg1);

  auto NumUsers = getUsersCountInSameBB(Reg0, MI);
  auto TmpReg = insertFill(MI, Reg0);
  TheStack.push(TmpReg, NumUsers + 1);

  insertSwap<BEFORE>(MI, Reg1);

  insertDup<BEFORE>(MI, Reg0);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_MU_IM_1U_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto NumUsers = getUsersCountInSameBB(Reg0, MI);
  insertFill(MI, Reg0);
  TheStack.push(Reg0, NumUsers + 1);

  insertFill(MI, Reg1);
  TheStack.push(Reg1, 1);

  insertDup<BEFORE>(MI, 1);
}

template<>
void EVMStackAllocation::handleInst<EVMStackAllocation::B_MU_IM_MU_IM>(
    MachineInstr& MI, Register Reg0, Register Reg1) {
  auto NumUsers = getUsersCountInSameBB(Reg1, MI);
  auto TmpReg = insertFill(MI, Reg1);
  TheStack.push(TmpReg, NumUsers + 1);
  insertDup<BEFORE>(MI, 0);

  NumUsers = getUsersCountInSameBB(Reg0, MI);
  TmpReg = insertFill(MI, Reg0);
  TheStack.push(TmpReg, NumUsers + 1);
  insertDup<BEFORE>(MI, 0);
}


Register EVMStackAllocation::getDefRegister(const MachineInstr &MI) {
  assert(MI.getNumExplicitDefs() == 1 &&
         "Supported only single definition instructions");
  return MI.defs().begin()->getReg();
}

void EVMStackAllocation::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<LiveIntervals>();
  MachineFunctionPass::getAnalysisUsage(AU);
}

FunctionPass *createEVMStackAllocationPass() {
  return new EVMStackAllocation();
}

} // namespace llvm
