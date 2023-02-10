//==-- EVMTargetStreamer.h - EVM Target Streamer -*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares EVM-specific target streamer classes.
/// These are for implementing support for target-specific assembly directives.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_EVM_MCTARGETDESC_EVMTARGETSTREAMER_H
#define LLVM_LIB_TARGET_EVM_MCTARGETDESC_EVMTARGETSTREAMER_H

#include "llvm/BinaryFormat/Wasm.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/MachineValueType.h"

namespace llvm {

class MCSymbolEVM;

/// EVM-specific streamer interface, to implement support
/// EVM-specific assembly directives.
class EVMTargetStreamer : public MCTargetStreamer {
public:
  explicit EVMTargetStreamer(MCStreamer &S);

  virtual void emitFunctionType(const MCSymbolEVM *Sym) = 0;
  virtual void emitType(const MCSymbolEVM *Sym, std::string_view Type) = 0;
};

/// This part is for ascii assembly output
class EVMTargetAsmStreamer final : public EVMTargetStreamer {
  formatted_raw_ostream &OS;

public:
  EVMTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);

  void emitFunctionType(const MCSymbolEVM *Sym) override;
  void emitType(const MCSymbolEVM *Sym, std::string_view Type) override;
};

/// This part is for null output
class EVMTargetNullStreamer final : public EVMTargetStreamer {
public:
  explicit EVMTargetNullStreamer(MCStreamer &S)
      : EVMTargetStreamer(S) {}

  void emitFunctionType(const MCSymbolEVM *Sym) override {}
  void emitType(const MCSymbolEVM *Sym, std::string_view Type) override {}
};

// TODO: add JSON output streamer.

} // end namespace llvm

#endif
