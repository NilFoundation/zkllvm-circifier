//===--- EVM.cpp - Implement EVM target feature support -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements EVM TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "EVM.h"
#include "Targets.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/StringRef.h"

using namespace clang;
using namespace clang::targets;

const Builtin::Info EVMTargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS)                                               \
  {#ID, TYPE, ATTRS, nullptr, ALL_LANGUAGES, nullptr},
#include "clang/Basic/BuiltinsEVM.def"
};

EVMTargetInfo::EVMTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
    : TargetInfo(Triple) {
  PointerWidth = 64;
  BoolWidth = 8;
  CharWidth = 8;
  IntWidth = 32;
  LongWidth = 64;
  LongLongWidth = 64;
  MaxBitIntWidth = 64;
  LongLongAlign = 32;
  Int256Align = 32;
  resetDataLayout("E-S64-a:0:32-p:64:64-i1:8:8-i8:8:8-i16:16:16-i32"
                  ":32:32-i64:32:32-i128:32:32-i256:32:32");
}

void EVMTargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__evm__");
  Builder.defineMacro("__EVM__");
  Builder.defineMacro("__ELF__");
  Builder.defineMacro("_LIBCPP_OBJECT_FORMAT_ELF");
  Builder.defineMacro("_LIBCPP_NO_RTTI");
  Builder.defineMacro("_LIBCPP_HAS_NO_THREADS");
  Builder.defineMacro("_LIBCPP_NO_EXCEPTIONS");
}

static constexpr llvm::StringLiteral ValidCPUNames[] = {"generic"};

bool EVMTargetInfo::isValidCPUName(StringRef Name) const {
  return llvm::is_contained(ValidCPUNames, Name);
}

void EVMTargetInfo::fillValidCPUList(SmallVectorImpl<StringRef> &Values) const {
  Values.append(std::begin(ValidCPUNames), std::end(ValidCPUNames));
}

ArrayRef<Builtin::Info> EVMTargetInfo::getTargetBuiltins() const {
  return llvm::makeArrayRef(BuiltinInfo, clang::EVM::LastTSBuiltin -
                                             Builtin::FirstTSBuiltin);
}

bool EVMTargetInfo::handleTargetFeatures(std::vector<std::string> &Features,
                                         DiagnosticsEngine &Diags) {
  return true;
}
