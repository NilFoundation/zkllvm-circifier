//=- EVMMachineFunctionInfo.h - EVM machine function info -----*- C++ -*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares EVM-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_EVM_EVMMACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_EVM_EVMMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

namespace llvm {

/// EVMMachineFunctionInfo - This class is derived from MachineFunctionInfo
/// and contains private EVM-specific information for each MachineFunction.
class EVMMachineFunctionInfo : public MachineFunctionInfo {
public:
  EVMMachineFunctionInfo(MachineFunction &MF): MF(MF) {}

  void setNumStackArgs(unsigned size) {
    NumStackArgs = size;
  }

  unsigned getNumStackArgs() {
    return NumStackArgs;
  }

  void setSpillSlots(unsigned N) {
    SpillsCount = N;
  }

  unsigned getFrameSizeInBytes() const {
    int64_t LocalsSize = MF.getFrameInfo().getStackSize();
    return SpillsCount * getStackSlotSizeInBytes() + LocalsSize;
  }

  static unsigned getStackSlotSizeInBytes() {
    return 8;
  }

private:
  MachineFunction &MF;
  unsigned NumStackArgs{0};
  unsigned SpillsCount{0};

};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_EVM_EVMMACHINEFUNCTIONINFO_H
