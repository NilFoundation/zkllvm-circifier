//===--- EVM.h - Declare EVM target feature support -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares EVM TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_EVM_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_EVM_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY EVMTargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];

public:
  EVMTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    HasEVMTypes = true;
    NoAsmVariants = true;
    SuitableAlign = 256;
    FloatAlign = 256;
    DoubleAlign = 256;
    LongDoubleAlign = 256;
    LargeArrayAlign = 256;
    SimdDefaultAlign = 256;
    PointerAlign = 256;
    BoolAlign = 256;
    IntAlign = 256;
    LongAlign = 256;
    LongLongAlign = 256;

    LargeArrayMinWidth = 128;
    SigAtomicType = SignedLong;

    FloatWidth = 256;
    DoubleWidth = 256;
    LongDoubleWidth = 256;
    LongDoubleFormat = &llvm::APFloat::IEEEquad();

    MaxAtomicPromoteWidth = MaxAtomicInlineWidth = 256;
    PointerWidth = 256;
    BoolWidth = 256;
    IntWidth = 256;
    LongWidth = 256;
    LongLongWidth = 256;

    SizeType = UnsignedLong;
    PtrDiffType = SignedLong;
    IntPtrType = SignedLong;
    WCharType = UnsignedLong;
    Char16Type = UnsignedLong;
    Char32Type = UnsignedLong;
    resetDataLayout("e-S256-i1:256:256-i8:256:256-i16:256:256-i32:256:256-"
                    "i64:256:256-i256:256:256-p:256:256-a:256:256");
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  bool hasFeature(StringRef Feature) const override {
    return Feature == "evm";
  }

  void setFeatureEnabled(llvm::StringMap<bool> &Features, StringRef Name,
                         bool Enabled) const override {
    Features[Name] = Enabled;
  }
  bool handleTargetFeatures(std::vector<std::string> &Features,
                            DiagnosticsEngine &Diags) override;

  ArrayRef<Builtin::Info> getTargetBuiltins() const override;

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const final {
    return false;
  }
  
  const char *getClobbers() const override { return ""; }

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  bool isValidGCCRegisterName(StringRef Name) const override { return true; }
  ArrayRef<const char *> getGCCRegNames() const override { return None; }

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
    return None;
  }

  bool allowDebugInfoForExternalRef() const override { return true; }

  CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {
    switch (CC) {
    default:
      return CCCR_Warning;
    case CC_C:
    case CC_OpenCLKernel:
      return CCCR_OK;
    }
  }

  bool isValidCPUName(StringRef Name) const override;

  void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override;

  bool setCPU(const std::string &Name) override {
    StringRef CPUName(Name);
    return isValidCPUName(CPUName);
  }
};
} // namespace targets
} // namespace clang
#endif // LLVM_CLANG_LIB_BASIC_TARGETS_EVM_H
