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

void AssignerTargetInfo::getTargetDefines(const LangOptions &Opts,
                                           MacroBuilder &Builder) const {
  Builder.defineMacro("__ASSIGNER__");

  // TODO(maksenov): remove this after supporting custom standard library
  Builder.defineMacro("__amd64__");
  Builder.defineMacro("__amd64");
  Builder.defineMacro("__x86_64");
  Builder.defineMacro("__x86_64__");
}

ArrayRef<Builtin::Info> AssignerTargetInfo::getTargetBuiltins() const {
    return None;
}
