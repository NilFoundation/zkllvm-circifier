//===--- Assigner.cpp - Implement Assigner target feature support -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements Assigner TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "Assigner.h"
#include "Targets.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

using namespace clang;
using namespace clang::targets;

const Builtin::Info AssignerTargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS)                                               \
  {#ID, TYPE, ATTRS, nullptr, HeaderDesc::NO_HEADER, ALL_LANGUAGES},
#include "clang/Basic/BuiltinsAssigner.def"
};

void AssignerTargetInfo::getTargetDefines(const LangOptions &Opts,
                                           MacroBuilder &Builder) const {
  Builder.defineMacro("__ASSIGNER__");
  Builder.defineMacro("__ELF__");
  Builder.defineMacro("_LIBCPP_OBJECT_FORMAT_ELF");
  Builder.defineMacro("_LIBCPP_NO_RTTI");
  Builder.defineMacro("_LIBCPP_HAS_NO_THREADS");
  Builder.defineMacro("_LIBCPP_NO_EXCEPTIONS");
  Builder.defineMacro("_LIBCPP_HAS_NO_INT256");
}

ArrayRef<Builtin::Info> AssignerTargetInfo::getTargetBuiltins() const {
    return llvm::ArrayRef(BuiltinInfo, clang::assigner::LastTSBuiltin -
                                             Builtin::FirstTSBuiltin);
}
