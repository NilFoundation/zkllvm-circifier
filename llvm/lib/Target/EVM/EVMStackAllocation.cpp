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
#include "EVMMachineFunctionInfo.h"
#include "EVMInstrInfo.h"
#include "EVMSubtarget.h"
#include "MCTargetDesc/EVMMCTargetDesc.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/ADT/PostOrderIterator.h"

#include <vector>

using namespace llvm;

namespace {

class StackElement {
public:

  static constexpr unsigned INVALID_REGISTER = std::numeric_limits<unsigned>::max();

  StackElement() = default;
  StackElement(Register R, uint16_t Users) : StackElement(R, Users, false) {}
  StackElement(Register R, uint16_t Users, bool IF) : Reg(R),
                                                      NumUsers(Users),
                                                      IsFixed(IF) {
    assert(R.isVirtual());
  }

  static StackElement createParameterItem(Register Reg) {
    return StackElement(Reg, std::numeric_limits<decltype(NumUsers)>::max(), true);
  }

  Register getReg() const {
    return Register::virtReg2Index(Reg);
  }

  bool isValid() const {
    return getReg() != INVALID_REGISTER;
  }

  void consume() {
    assert(NumUsers != 0);
    NumUsers--;
  }

  void addUser() {
    NumUsers++;
  }

  operator bool() const {
    return getReg() != INVALID_REGISTER;
  }

//private:
  Register Reg{INVALID_REGISTER};
  uint16_t NumUsers{0};
  bool IsFixed{false};
};

class Stack {
public:

  unsigned size() const {
    return Vector.size();
  }

  bool empty() const {
    return size() == 0;
  }

  void push(StackElement E) {
    errs() << "[stack] push: " << utohexstr(E.getReg()) << '\n';
    Vector.push_back(E);
  }

  void pop() {
    errs() << "[stack] pop: " << utohexstr(top().getReg()) << '\n';
    Vector.pop_back();
  }

  StackElement& top() {
    return Vector.back();
  }

  void dup(int Index) {
    StackElement NewElem(operator[](Index).getReg(), 0);
    push(NewElem);
  }
  void swap(int Index) {
    assert(Index >= 0 && static_cast<unsigned>(Index) < size());
    std::iter_swap(Vector.rbegin(), Vector.rbegin() + Index);
  }

  int find(Register Reg) {
    for (auto It = Vector.rbegin(); It != Vector.rend(); ++It) {
      if (It->getReg() == Reg)
        return std::distance(Vector.rbegin(), It);
    }
    return -1;
  }

  StackElement& operator[](unsigned Index) {
    assert(Index < size());
    return Vector[size() - Index - 1];
  }

  static bool isValidIndex(int I) {
    return I >= 0;
  }

  template<typename T>
  void dump(T& OS) {
    OS << "Stack: ";
    const char* Sep = "";
    for (unsigned i = 0; i < size(); i++) {
      OS << Sep << i << ":" << operator[](i).getReg();
      Sep = ", ";
    }
    OS << '\n';
  }

private:
//  std::vector<StackElement> Stack;
  SmallVector<StackElement, 16> Vector;
  StackElement InvalidElement;
};

template<typename T>
T &operator<<(T &OS, Stack& S) {
  S.dump(OS);
  return OS;
}

class EVMStackAllocation : public MachineFunctionPass {
public:
  static inline char ID = 0;

  EVMStackAllocation() : MachineFunctionPass(ID) {
    initializeEVMStackAllocationPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override;

private:
  unsigned getUsersCountInSameBB(unsigned reg, const MachineInstr &MI) const;
  bool hasUsersInOtherBB(unsigned reg, const MachineInstr &MI) const;
  void handleUse(unsigned OpndIndex, MachineInstr &MI, const MachineOperand &MO);
  void prepareOperands(MachineInstr &MI);
  void executeInstruction(MachineInstr &MI);
  void cleanupStack(MachineInstr &MI);
  unsigned getMemorySlot(Register Reg) {
    auto It = MemorySlotsMap.find(Reg);
    if (It != MemorySlotsMap.end()) {
      return It->second;
    }
    MemorySlotsMap[Reg] = CurrentMemorySlot++;
    return CurrentMemorySlot - 1;
  }

  void emitDup(MachineInstr &MI, int Index) {
    auto Inst =
        BuildMI(*F, MI.getDebugLoc(), TII->get(EVM::DUP_r)).addImm(Index);
    MI.getParent()->insert(MachineBasicBlock::iterator(MI), Inst);
    TheStack.dup(Index);
  }
  void emitSwap(MachineInstr &MI, int Index) {
    auto Inst =
        BuildMI(*F, MI.getDebugLoc(), TII->get(EVM::SWAP_r)).addImm(Index);
    MI.getParent()->insert(MachineBasicBlock::iterator(MI), Inst);
    TheStack.swap(Index);
  }
  void emitPop(MachineInstr &MI) {
    auto Inst = BuildMI(*F, MI.getDebugLoc(), TII->get(EVM::POP_r));
    MI.getParent()->insert(MachineBasicBlock::iterator(MI), Inst);
    TheStack.pop();
  }

  void emitMemoryLoad(MachineInstr &MI) {
    auto Reg = getDefRegister(MI);
    auto Slot = getMemorySlot(Reg);
    MachineInstrBuilder getlocal =
        BuildMI(*MI.getParent(), MI, MI.getDebugLoc(), TII->get(EVM::pGETLOCAL_r), Reg)
            .addImm(Slot);
    LIS->InsertMachineInstrInMaps(*getlocal);
  }

  void emitMemoryStore(MachineInstr &MI) {

  }

  static Register getDefRegister(MachineInstr &MI);

private:
  Stack TheStack;
  LiveIntervals *LIS{nullptr};
  const EVMInstrInfo *TII{nullptr};
  MachineFunction *F{nullptr};
  MachineRegisterInfo *MRI{nullptr};
  EVMMachineFunctionInfo *MFI{nullptr};
  unsigned CurrentMemorySlot{0};
  std::unordered_map<unsigned, unsigned> MemorySlotsMap;
};

}

/*******************************************************************************
 * Implementation
 */

bool EVMStackAllocation::runOnMachineFunction(MachineFunction &MF) {
  TII = MF.getSubtarget<EVMSubtarget>().getInstrInfo();
  LIS = &getAnalysis<LiveIntervals>();
  MRI = &MF.getRegInfo();
  MFI = MF.getInfo<EVMMachineFunctionInfo>();
  F = &MF;

  MachineBasicBlock &FirstBB = MF.front();
  for (auto& MI : FirstBB) {
    if (MI.getOpcode() == EVM::pSTACKARG_r) {
      auto Reg = getDefRegister(MI);
      if (hasUsersInOtherBB(Reg, MI)) {
        emitMemoryStore(MI);
      }
      TheStack.push(StackElement::createParameterItem(getDefRegister(MI)));
    }
  }


  ReversePostOrderTraversal<MachineFunction *> RPO(&MF);

  for (MachineBasicBlock* MBB : RPO) {
    for (auto& MI : *MBB) {
      if (MI.getOpcode() == EVM::pSTACKARG_r) {
        continue;
      }
      errs() << "INST: "; MI.dump();
      prepareOperands(MI);
      executeInstruction(MI);
      cleanupStack(MI);
      errs() << TheStack << '\n';
    }
  }
  return true;
}

void EVMStackAllocation::prepareOperands(MachineInstr &MI) {
  unsigned OpndIndex = MI.getNumExplicitOperands() - MI.getNumExplicitDefs() - 1;
  bool RegIsLegal = true;
  for (const MachineOperand &MOP : reverse(MI.explicit_uses())) {
    if (MOP.isReg()) {
      assert(RegIsLegal);
      handleUse(OpndIndex, MI, MOP);
    } else {
      assert(OpndIndex == 0);
      assert(MI.getOpcode() == EVM::PUSH32_r ||
             MI.getOpcode() == EVM::JUMP_r ||
             MI.getOpcode() == EVM::JUMPI_r ||
             MI.getOpcode() == EVM::pSTACKARG_r);
      RegIsLegal = false;
    }
    OpndIndex--;
  }
}

void EVMStackAllocation::handleUse(unsigned OpndIndex, MachineInstr &MI, const MachineOperand &MOP) {
  int Index = TheStack.find(MOP.getReg());
  if (Stack::isValidIndex(Index)) {
    auto& Item = TheStack[Index];
    assert(Item.NumUsers != 0);
    if (OpndIndex == static_cast<unsigned>(Index) && Item.NumUsers == 1) {
      Item.consume();
    } else {
      Item.consume();
      emitDup(MI, Index);
    }
  } else {
    emitMemoryLoad(MI);
  }
}

void EVMStackAllocation::executeInstruction(MachineInstr &MI) {
  for (const MachineOperand &MOP : reverse(MI.explicit_uses())) {
    if (!MOP.isReg()) {
      continue;
    }
    assert(!TheStack.empty() && "Stack underflow");
    auto& Item = TheStack.top();
    assert(Item.NumUsers == 1 && "Wrong number of uses");
    TheStack.pop();
  }
  if (MI.getNumDefs() != 0) {
    auto Reg = MI.defs().begin()->getReg();
    if (auto NumUsers = getUsersCountInSameBB(Reg, MI); NumUsers != 0) {
      TheStack.push(StackElement(Reg, NumUsers));
    } else {
      auto Pop = BuildMI(*F, MI.getDebugLoc(), TII->get(EVM::POP_r));
      MI.getParent()->insert(MachineBasicBlock::iterator(MI), Pop);
    }
  }
}

unsigned EVMStackAllocation::getUsersCountInSameBB(
    unsigned reg, const MachineInstr &MI) const {
  SlotIndex FstUse = LIS->getInstructionIndex(MI);
  unsigned UsesCount = 0;

  // Iterate over uses and see if any use exists in the same BB.
  for (auto& Use : MRI->use_nodbg_instructions(reg)) {
    SlotIndex SI = LIS->getInstructionIndex(Use);
    SlotIndex EndOfMBBSI = LIS->getMBBEndIdx(Use.getParent());

    // Check if SI lies in between FstUSE and EndOfMBBSI
    if (SlotIndex::isEarlierInstr(FstUse, SI) &&
        SlotIndex::isEarlierInstr(SI, EndOfMBBSI)) {
      UsesCount++;
    }
  }

  return UsesCount;
}

bool EVMStackAllocation::hasUsersInOtherBB(unsigned int reg, const MachineInstr &MI) const {
  auto MBB = MI.getParent();
  for (auto& Use : MRI->use_nodbg_instructions(reg)) {
    if (Use.getParent() != MBB) {
      return true;
    }
  }
  return false;
}
void EVMStackAllocation::cleanupStack(MachineInstr &MI) {
  if (TheStack.empty()) {
    return;
  }
  for (auto& Item = TheStack.top();
       !TheStack.empty() && Item.NumUsers == 0;
       Item = TheStack.top()) {
    emitPop(MI);
  }
}

Register EVMStackAllocation::getDefRegister(MachineInstr &MI) {
  assert(MI.getNumExplicitDefs() == 1 &&
         "Supported only single definition instructions");
  return MI.defs().begin()->getReg();
}

void EVMStackAllocation::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<LiveIntervals>();
  MachineFunctionPass::getAnalysisUsage(AU);
}

INITIALIZE_PASS(EVMStackAllocation, "evm-stackallocation",
                "EVM Stack Allocation", false, false)

FunctionPass *llvm::createEVMStackAllocationPass() {
  return new EVMStackAllocation();
}
