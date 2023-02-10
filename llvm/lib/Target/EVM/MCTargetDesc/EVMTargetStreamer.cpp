//==-- EVMTargetStreamer.cpp - EVM Target Streamer Methods --=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines EVM-specific target streamer classes.
/// These are for implementing support for target-specific assembly directives.
///
//===----------------------------------------------------------------------===//

#include "EVMTargetStreamer.h"
#include "MCTargetDesc/EVMInstPrinter.h"
#include "EVMMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbolEVM.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

// The basic target streamer.
EVMTargetStreamer::EVMTargetStreamer(MCStreamer &S)
    : MCTargetStreamer(S) {}

EVMTargetAsmStreamer::EVMTargetAsmStreamer(
    MCStreamer &S, formatted_raw_ostream &OS)
    : EVMTargetStreamer(S), OS(OS) {}

void EVMTargetAsmStreamer::emitFunctionType(const MCSymbolEVM *Sym) {
  OS << "\t.functype\t" << Sym->getName() << ", (";
  auto Sep = "";
  for (auto Param : Sym->getSignature()->Params) {
    OS << Sep << ValTypeToString(Param);
    Sep = ", ";
  }
  Sep = "";
  OS << ")->(";
  for (auto Return : Sym->getSignature()->Returns) {
    OS << Sep << ValTypeToString(Return);
    Sep = ", ";
  }
  OS << ")\n";
}

void EVMTargetAsmStreamer::emitType(const MCSymbolEVM *Sym, std::string_view Type) {
  OS << "\t.type\t" << Sym->getName() << "," << Type << '\n';
}