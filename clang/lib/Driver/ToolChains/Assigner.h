//===--- Assigner.h - Assigner ToolChain Implementations --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ASSIGNER_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ASSIGNER_H

#include "Gnu.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"
#include "llvm/Support/Path.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Assigner final : public ToolChain {
public:
  Assigner(const Driver &D, const llvm::Triple &Triple,
              const llvm::opt::ArgList &Args) : ToolChain(D, Triple, Args) {}

private:
  bool isPICDefault() const override {
    return false;
  }
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override {
    return false;
  }
  bool isPICDefaultForced() const override {
    return false;
  }
  bool hasBlocksRuntime() const override {
    return false;
  }
  bool SupportsProfiling() const override {
    return false;
  }
  void
  addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
                        llvm::opt::ArgStringList &CC1Args,
                        Action::OffloadKind DeviceOffloadKind) const override {}
  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override {
    if (DriverArgs.hasArg(options::OPT_nostdinc, options::OPT_nostdincxx,
                          options::OPT_nostdlibinc)) {
      return;
    }
    addExternCSystemInclude(DriverArgs, CC1Args, "/usr/lib/zkllvm/include/libc");
  }
  void AddClangCXXStdlibIncludeArgs(
      const llvm::opt::ArgList &DriverArgs,
      llvm::opt::ArgStringList &CC1Args) const override {
    addSystemInclude(DriverArgs, CC1Args, "/usr/lib/zkllvm/include/c++/v1");
    SmallString<128> ResourceDirInclude(getDriver().ResourceDir);
    llvm::sys::path::append(ResourceDirInclude, "include");
    addSystemInclude(DriverArgs, CC1Args, ResourceDirInclude);
  }

};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ASSIGNER_H
