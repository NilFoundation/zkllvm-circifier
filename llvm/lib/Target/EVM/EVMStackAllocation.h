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
#ifndef LLVM_EVMSTACKALLOCATION_H
#define LLVM_EVMSTACKALLOCATION_H

#include "EVM.h"
#include "EVMUtils.h"
#include "EVMStack.h"
#include "EVMMachineFunctionInfo.h"
#include "EVMInstrInfo.h"
#include "EVMSubtarget.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/ADT/SmallBitVector.h"

namespace llvm {

class EVMStackAllocation : public MachineFunctionPass {

public:
  static inline char ID = 0;

  EVMStackAllocation();

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override;

private:
  enum Insert { NONE, AFTER, BEFORE };

  unsigned getUsersCountInSameBB(unsigned reg, const MachineInstr &MI) const;
  bool hasUsersInOtherBB(const MachineInstr &MI) const;
  void handleOperands(MachineInstr &MI);
  void handleArguments();
  void executeInstruction(MachineInstr &MI);
  void cleanupStack(MachineInstr &MI, bool DeepClean);

  void insertAfterCurrent(MachineInstr &MI) {
    CurrentPosition->getParent()->insertAfter(CurrentPosition, &MI);
    CurrentPosition = MI.getIterator();
  }

  template<Insert Position>
  void insertInstruction(MachineInstr &MI, MachineInstr &InsertedMI) {
    if constexpr (Position == Insert::AFTER) {
      insertAfterCurrent(InsertedMI);
    } else if constexpr (Position == Insert::BEFORE) {
      MI.getParent()->insert(MachineBasicBlock::iterator(MI), &InsertedMI);
    }
  }

  template<Insert Position>
  MachineInstr* insertDup(MachineInstr &MI, int Index, bool Consume = true) {
    auto Inst =
        BuildMI(*MF, MI.getDebugLoc(), TII->get(EVM::DUP_r)).addImm(Index + 1);
    insertInstruction<Position>(MI, *Inst);
    if (Consume) {
      TheStack[Index].consume();
    }
    TheStack.dup(Index).addUsers(1);
    return Inst;
  }

  template<Insert Position>
  MachineInstr* insertDup(MachineInstr &MI, Register Reg) {
    return insertDup<Position>(MI, TheStack.find(Reg));
  }

  template<Insert Position>
  MachineInstr* insertSwap(MachineInstr &MI, int Index) {
    if (Index == 0) {
      return nullptr;
    }
    auto Inst =
        BuildMI(*MF, MI.getDebugLoc(), TII->get(EVM::SWAP_r)).addImm(Index);
    insertInstruction<Position>(MI, *Inst);
    TheStack.swap(Index);
    return Inst;
  }

  template<Insert Position>
  MachineInstr* insertSwap(MachineInstr &MI, Register Reg) {
    return insertSwap<Position>(MI, TheStack.find(Reg));
  }

  template<Insert Position>
  void insertPop(MachineInstr &MI) {
    auto Inst = BuildMI(*MF, MI.getDebugLoc(), TII->get(EVM::POP_r));
    insertInstruction<Position>(MI, *Inst);
    TheStack.pop();
  }

  void insertSpill(MachineInstr &MI);
  Register insertFill(MachineInstr &MI, Register Reg);

  enum OperandsPatternEnum {
    OP0_ONE_USE   = (1 << 0), // Operand 0 has only one use
    OP0_IN_STACK  = (1 << 1), // Operand 0 is on the stack, otherwise in memory
    OP1_EXIST     = (1 << 2), // Operand 1 is exist
    OP1_ONE_USE   = (1 << 3), // Operand 1 has only one use
    OP1_IN_STACK  = (1 << 4), // Operand 1 is on the stack, otherwise in memory
    SIZE          = (1 << 5),
  };

  // Pattern consists of the following items:
  //   Operation: U - Unary, B - Binary
  //   Operand's users: 1U - one user, MU - multiple users
  //   Location: IS - in stack, IM - in memory
#define LOCATION_PATTERNS_LIST(M) \
  M(U_1U_IS, OP0_ONE_USE | OP0_IN_STACK) \
  M(U_MU_IS, OP0_IN_STACK) \
  M(U_1U_IM, OP0_ONE_USE) \
  M(U_MU_IM, 0) \
  M(B_1U_IS_1U_IS, U_1U_IS | OP1_EXIST | OP1_ONE_USE | OP1_IN_STACK) \
  M(B_1U_IS_MU_IS, U_1U_IS | OP1_EXIST | OP1_IN_STACK) \
  M(B_1U_IS_1U_IM, U_1U_IS | OP1_EXIST | OP1_ONE_USE) \
  M(B_1U_IS_MU_IM, U_1U_IS | OP1_EXIST)  \
  M(B_MU_IS_1U_IS, U_MU_IS | OP1_EXIST | OP1_ONE_USE | OP1_IN_STACK) \
  M(B_MU_IS_MU_IS, U_MU_IS | OP1_EXIST | OP1_IN_STACK) \
  M(B_MU_IS_1U_IM, U_MU_IS | OP1_EXIST | OP1_ONE_USE) \
  M(B_MU_IS_MU_IM, U_MU_IS | OP1_EXIST)  \
  M(B_1U_IM_1U_IS, U_1U_IM | OP1_EXIST | OP1_ONE_USE | OP1_IN_STACK) \
  M(B_1U_IM_MU_IS, U_1U_IM | OP1_EXIST | OP1_IN_STACK) \
  M(B_1U_IM_1U_IM, U_1U_IM | OP1_EXIST | OP1_ONE_USE) \
  M(B_1U_IM_MU_IM, U_1U_IM | OP1_EXIST)  \
  M(B_MU_IM_1U_IS, U_MU_IM | OP1_EXIST | OP1_ONE_USE | OP1_IN_STACK) \
  M(B_MU_IM_MU_IS, U_MU_IM | OP1_EXIST | OP1_IN_STACK) \
  M(B_MU_IM_1U_IM, U_MU_IM | OP1_EXIST | OP1_ONE_USE) \
  M(B_MU_IM_MU_IM, U_MU_IM | OP1_EXIST)

#define DEF_HANDLER(NAME, VALUE) \
  static constexpr unsigned NAME = VALUE;
  LOCATION_PATTERNS_LIST(DEF_HANDLER)
#undef DEF_HANDLER

  template<unsigned Pattern>
  void handleInst(MachineInstr& MI, Register Reg0, Register Reg1);

#define DEF_HANDLER(NAME, VALUE) \
      template<> void handleInst<NAME>(MachineInstr& MI, Register Reg0, Register Reg1);
  LOCATION_PATTERNS_LIST(DEF_HANDLER)
#undef DEF_HANDLER

  const char* getPattternName(unsigned Pattern) {
    switch (Pattern) {
#define DEF_HANDLER(NAME, VALUE) \
      case VALUE: return #NAME;
      LOCATION_PATTERNS_LIST(DEF_HANDLER)
#undef DEF_HANDLER
    default:
      return "UNHANDLED PATTERN";
    }
  }

  using OperandsHandler = void (EVMStackAllocation::*)(MachineInstr&, Register, Register);

  static IndexedMap<OperandsHandler> createMap() {
    IndexedMap<OperandsHandler> Map{nullptr};
#define DEF_HANDLER(NAME, VALUE) \
      Map[VALUE] = &EVMStackAllocation::handleInst<NAME>;
    LOCATION_PATTERNS_LIST(DEF_HANDLER)
#undef DEF_HANDLER
    return Map;
  }

  static Register getDefRegister(const MachineInstr &MI);

private:
  Stack TheStack;
  LiveIntervals *LIS{nullptr};
  const EVMInstrInfo *TII{nullptr};
  MachineFunction *MF{nullptr};
  MachineRegisterInfo *MRI{nullptr};
  EVMMachineFunctionInfo *MFI{nullptr};
  static constexpr int INVALID_SLOT = -1;
  IndexedMap<int, VirtReg2IndexFunctor> MemorySlotsMap{INVALID_SLOT};
  // This map is used to handle case when vreg is used multiple time in one
  // instruction, e.g: `foo(var1, var1);`
  std::unordered_map<unsigned, bool> SeenRegisters;
  IndexedMap<OperandsHandler> OperandsPatternMap{nullptr};
  MachineBasicBlock::iterator CurrentPosition;
  unsigned AllocatedSlotNumber{0};
};


} // namespace llvm

#endif // LLVM_EVMSTACKALLOCATION_H
