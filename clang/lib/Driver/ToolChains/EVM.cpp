//===--- EVM.cpp - EVM ToolChain Implementation -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EVM.h"
#include "CommonArgs.h"
#include "Gnu.h"
#include "clang/Basic/Version.h"
#include "clang/Config/config.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang::driver::toolchains;
using namespace clang;
using namespace llvm::opt;

/// Following the conventions in https://wiki.debian.org/Multiarch/Tuples,
/// we remove the vendor field to form the multiarch triple.
std::string EVM::getMultiarchTriple(const Driver &D,
                                            const llvm::Triple &TargetTriple,
                                            StringRef SysRoot) const {
    return (TargetTriple.getArchName() + "-" +
            TargetTriple.getOSAndEnvironmentName()).str();
}

std::string evm::Linker::getLinkerPath(const ArgList &Args) const {
  const ToolChain &ToolChain = getToolChain();
  if (const Arg* A = Args.getLastArg(options::OPT_fuse_ld_EQ)) {
    StringRef UseLinker = A->getValue();
    if (!UseLinker.empty()) {
      if (llvm::sys::path::is_absolute(UseLinker) &&
          llvm::sys::fs::can_execute(UseLinker))
        return std::string(UseLinker);

      // Accept 'lld', and 'ld' as aliases for the default linker
      if (UseLinker != "lld" && UseLinker != "ld")
        ToolChain.getDriver().Diag(diag::err_drv_invalid_linker_name)
            << A->getAsString(Args);
    }
  }

  return ToolChain.GetProgramPath(ToolChain.getDefaultLinker());
}

void evm::Linker::ConstructJob(Compilation &C, const JobAction &JA,
                                const InputInfo &Output,
                                const InputInfoList &Inputs,
                                const ArgList &Args,
                                const char *LinkingOutput) const {

  const ToolChain &ToolChain = getToolChain();
  const char *Linker = Args.MakeArgString(getLinkerPath(Args));
  ArgStringList CmdArgs;

  CmdArgs.push_back("-m");
  CmdArgs.push_back("evm");

  if (Args.hasArg(options::OPT_s))
    CmdArgs.push_back("--strip-all");

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  Args.AddAllArgs(CmdArgs, options::OPT_u);
  ToolChain.AddFilePathLibArgs(Args, CmdArgs);

  const char *Crt1 = "crt1.o";
  const char *Entry = nullptr;

  // If crt1-command.o exists, it supports new-style commands, so use it.
  // Otherwise, use the old crt1.o. This is a temporary transition measure.
  // Once WASI libc no longer needs to support LLVM versions which lack
  // support for new-style command, it can make crt1.o the same as
  // crt1-command.o. And once LLVM no longer needs to support WASI libc
  // versions before that, it can switch to using crt1-command.o.
  if (ToolChain.GetFilePath("crt1-command.o") != "crt1-command.o")
    Crt1 = "crt1-command.o";

  if (const Arg *A = Args.getLastArg(options::OPT_mexec_model_EQ)) {
    StringRef CM = A->getValue();
    if (CM == "command") {
      // Use default values.
    } else if (CM == "reactor") {
      Crt1 = "crt1-reactor.o";
      Entry = "_initialize";
    } else {
      ToolChain.getDriver().Diag(diag::err_drv_invalid_argument_to_option)
          << CM << A->getOption().getName();
    }
  }
  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nostartfiles))
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(Crt1)));
  if (Entry) {
    CmdArgs.push_back(Args.MakeArgString("--entry"));
    CmdArgs.push_back(Args.MakeArgString(Entry));
  }

  AddLinkerInputs(ToolChain, Inputs, Args, CmdArgs, JA);

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs)) {
    if (ToolChain.ShouldLinkCXXStdlib(Args))
      ToolChain.AddCXXStdlibLibArgs(Args, CmdArgs);

    if (Args.hasArg(options::OPT_pthread)) {
      CmdArgs.push_back("-lpthread");
      CmdArgs.push_back("--shared-memory");
    }

    CmdArgs.push_back("-lc");
    AddRunTimeLibs(ToolChain, ToolChain.getDriver(), CmdArgs, Args);
  }

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  C.addCommand(std::make_unique<Command>(JA, *this,
                                         ResponseFileSupport::AtFileCurCP(),
                                         Linker, CmdArgs, Inputs, Output));

  // When optimizing, if evm-opt is available, run it.
  if (Arg *A = Args.getLastArg(options::OPT_O_Group)) {
    auto evmOptPath = ToolChain.GetProgramPath("evm-opt");
    if (evmOptPath != "evm-opt") {
      StringRef OOpt = "s";
      if (A->getOption().matches(options::OPT_O4) ||
          A->getOption().matches(options::OPT_Ofast))
        OOpt = "4";
      else if (A->getOption().matches(options::OPT_O0))
        OOpt = "0";
      else if (A->getOption().matches(options::OPT_O))
        OOpt = A->getValue();

      if (OOpt != "0") {
        const char *evmOpt = Args.MakeArgString(evmOptPath);
        ArgStringList CmdArgs;
        CmdArgs.push_back(Output.getFilename());
        CmdArgs.push_back(Args.MakeArgString(llvm::Twine("-O") + OOpt));
        CmdArgs.push_back("-o");
        CmdArgs.push_back(Output.getFilename());
        C.addCommand(std::make_unique<Command>(
            JA, *this, ResponseFileSupport::AtFileCurCP(), evmOpt, CmdArgs,
            Inputs, Output));
      }
    }
  }
}

/// Given a base library directory, append path components to form the
/// LTO directory.
static std::string AppendLTOLibDir(const std::string &Dir) {
    // The version allows the path to be keyed to the specific version of
    // LLVM in used, as the bitcode format is not stable.
    return Dir + "/llvm-lto/" LLVM_VERSION_STRING;
}

EVM::EVM(const Driver &D, const llvm::Triple &Triple,
                         const llvm::opt::ArgList &Args)
    : ToolChain(D, Triple, Args) {

  assert(Triple.isArch32Bit() != Triple.isArch64Bit());

  getProgramPaths().push_back(getDriver().getInstalledDir());

  auto SysRoot = getDriver().SysRoot;
  if (getTriple().getOS() == llvm::Triple::UnknownOS) {
    // Theoretically an "unknown" OS should mean no standard libraries, however
    // it could also mean that a custom set of libraries is in use, so just add
    // /lib to the search path. Disable multiarch in this case, to discourage
    // paths containing "unknown" from acquiring meanings.
    getFilePaths().push_back(SysRoot + "/lib");
  } else {
    const std::string MultiarchTriple =
        getMultiarchTriple(getDriver(), Triple, SysRoot);
    if (D.isUsingLTO()) {
      // For LTO, enable use of lto-enabled sysroot libraries too, if available.
      // Note that the directory is keyed to the LLVM revision, as LLVM's
      // bitcode format is not stable.
      auto Dir = AppendLTOLibDir(SysRoot + "/lib/" + MultiarchTriple);
      getFilePaths().push_back(Dir);
    }
    getFilePaths().push_back(SysRoot + "/lib/" + MultiarchTriple);
  }
}

bool EVM::IsMathErrnoDefault() const { return false; }

bool EVM::IsObjCNonFragileABIDefault() const { return true; }

bool EVM::UseObjCMixedDispatch() const { return true; }

bool EVM::isPICDefault() const { return false; }

bool EVM::isPIEDefault(const llvm::opt::ArgList &Args) const {
  return false;
}

bool EVM::isPICDefaultForced() const { return false; }

bool EVM::IsIntegratedAssemblerDefault() const { return true; }

bool EVM::hasBlocksRuntime() const { return false; }

// TODO: Support profiling.
bool EVM::SupportsProfiling() const { return false; }

bool EVM::HasNativeLLVMSupport() const { return true; }

void EVM::addClangTargetOptions(const ArgList &DriverArgs,
                                        ArgStringList &CC1Args,
                                        Action::OffloadKind) const {
  if (!DriverArgs.hasFlag(clang::driver::options::OPT_fuse_init_array,
                          options::OPT_fno_use_init_array, true))
    CC1Args.push_back("-fno-use-init-array");

  // '-pthread' implies atomics, bulk-memory, mutable-globals, and sign-ext
  if (DriverArgs.hasFlag(options::OPT_pthread, options::OPT_no_pthread,
                         false)) {
    if (DriverArgs.hasFlag(options::OPT_mno_atomics, options::OPT_matomics,
                           false))
      getDriver().Diag(diag::err_drv_argument_not_allowed_with)
          << "-pthread"
          << "-mno-atomics";
    if (DriverArgs.hasFlag(options::OPT_mno_bulk_memory,
                           options::OPT_mbulk_memory, false))
      getDriver().Diag(diag::err_drv_argument_not_allowed_with)
          << "-pthread"
          << "-mno-bulk-memory";
    if (DriverArgs.hasFlag(options::OPT_mno_mutable_globals,
                           options::OPT_mmutable_globals, false))
      getDriver().Diag(diag::err_drv_argument_not_allowed_with)
          << "-pthread"
          << "-mno-mutable-globals";
    if (DriverArgs.hasFlag(options::OPT_mno_sign_ext, options::OPT_msign_ext,
                           false))
      getDriver().Diag(diag::err_drv_argument_not_allowed_with)
          << "-pthread"
          << "-mno-sign-ext";
    CC1Args.push_back("-target-feature");
    CC1Args.push_back("+atomics");
    CC1Args.push_back("-target-feature");
    CC1Args.push_back("+bulk-memory");
    CC1Args.push_back("-target-feature");
    CC1Args.push_back("+mutable-globals");
    CC1Args.push_back("-target-feature");
    CC1Args.push_back("+sign-ext");
  }

  if (!DriverArgs.hasFlag(options::OPT_mmutable_globals,
                          options::OPT_mno_mutable_globals, false)) {
    // -fPIC implies +mutable-globals because the PIC ABI used by the linker
    // depends on importing and exporting mutable globals.
    llvm::Reloc::Model RelocationModel;
    unsigned PICLevel;
    bool IsPIE;
    std::tie(RelocationModel, PICLevel, IsPIE) =
        ParsePICArgs(*this, DriverArgs);
    if (RelocationModel == llvm::Reloc::PIC_) {
      if (DriverArgs.hasFlag(options::OPT_mno_mutable_globals,
                             options::OPT_mmutable_globals, false)) {
        getDriver().Diag(diag::err_drv_argument_not_allowed_with)
            << "-fPIC"
            << "-mno-mutable-globals";
      }
      CC1Args.push_back("-target-feature");
      CC1Args.push_back("+mutable-globals");
    }
  }

  if (DriverArgs.getLastArg(options::OPT_fevm_exceptions)) {
    // '-fevm-exceptions' is not compatible with '-mno-exception-handling'
    if (DriverArgs.hasFlag(options::OPT_mno_exception_handing,
                           options::OPT_mexception_handing, false))
      getDriver().Diag(diag::err_drv_argument_not_allowed_with)
          << "-fevm-exceptions"
          << "-mno-exception-handling";
    // '-fevm-exceptions' is not compatible with
    // '-mllvm -enable-emscripten-cxx-exceptions'
    for (const Arg *A : DriverArgs.filtered(options::OPT_mllvm)) {
      if (StringRef(A->getValue(0)) == "-enable-emscripten-cxx-exceptions")
        getDriver().Diag(diag::err_drv_argument_not_allowed_with)
            << "-fevm-exceptions"
            << "-mllvm -enable-emscripten-cxx-exceptions";
    }
    // '-fevm-exceptions' implies exception-handling feature
    CC1Args.push_back("-target-feature");
    CC1Args.push_back("+exception-handling");
    // Backend needs -evm-enable-eh to enable evm EH
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-evm-enable-eh");
  }

  for (const Arg *A : DriverArgs.filtered(options::OPT_mllvm)) {
    StringRef Opt = A->getValue(0);
    if (Opt.startswith("-emscripten-cxx-exceptions-allowed")) {
      // '-mllvm -emscripten-cxx-exceptions-allowed' should be used with
      // '-mllvm -enable-emscripten-cxx-exceptions'
      bool EmEHArgExists = false;
      for (const Arg *A : DriverArgs.filtered(options::OPT_mllvm)) {
        if (StringRef(A->getValue(0)) == "-enable-emscripten-cxx-exceptions") {
          EmEHArgExists = true;
          break;
        }
      }
      if (!EmEHArgExists)
        getDriver().Diag(diag::err_drv_argument_only_allowed_with)
            << "-mllvm -emscripten-cxx-exceptions-allowed"
            << "-mllvm -enable-emscripten-cxx-exceptions";

      // Prevent functions specified in -emscripten-cxx-exceptions-allowed list
      // from being inlined before reaching the evm backend.
      StringRef FuncNamesStr = Opt.split('=').second;
      SmallVector<StringRef, 4> FuncNames;
      FuncNamesStr.split(FuncNames, ',');
      for (auto Name : FuncNames) {
        CC1Args.push_back("-mllvm");
        CC1Args.push_back(DriverArgs.MakeArgString("--force-attribute=" + Name +
                                                   ":noinline"));
      }
    }

    if (Opt.startswith("-evm-enable-sjlj")) {
      // '-mllvm -evm-enable-sjlj' is not compatible with
      // '-mno-exception-handling'
      if (DriverArgs.hasFlag(options::OPT_mno_exception_handing,
                             options::OPT_mexception_handing, false))
        getDriver().Diag(diag::err_drv_argument_not_allowed_with)
            << "-mllvm -evm-enable-sjlj"
            << "-mno-exception-handling";
      // '-mllvm -evm-enable-sjlj' is not compatible with
      // '-mllvm -enable-emscripten-cxx-exceptions'
      // because we don't allow Emscripten EH + evm SjLj
      for (const Arg *A : DriverArgs.filtered(options::OPT_mllvm)) {
        if (StringRef(A->getValue(0)) == "-enable-emscripten-cxx-exceptions")
          getDriver().Diag(diag::err_drv_argument_not_allowed_with)
              << "-mllvm -evm-enable-sjlj"
              << "-mllvm -enable-emscripten-cxx-exceptions";
      }
      // '-mllvm -evm-enable-sjlj' is not compatible with
      // '-mllvm -enable-emscripten-sjlj'
      for (const Arg *A : DriverArgs.filtered(options::OPT_mllvm)) {
        if (StringRef(A->getValue(0)) == "-enable-emscripten-sjlj")
          getDriver().Diag(diag::err_drv_argument_not_allowed_with)
              << "-mllvm -evm-enable-sjlj"
              << "-mllvm -enable-emscripten-sjlj";
      }
      // '-mllvm -evm-enable-sjlj' implies exception-handling feature
      CC1Args.push_back("-target-feature");
      CC1Args.push_back("+exception-handling");
      // Backend needs '-exception-model=evm' to use evm EH instructions
      CC1Args.push_back("-exception-model=evm");
    }
  }
}

ToolChain::RuntimeLibType EVM::GetDefaultRuntimeLibType() const {
  return ToolChain::RLT_CompilerRT;
}

ToolChain::CXXStdlibType
EVM::GetCXXStdlibType(const ArgList &Args) const {
  if (Arg *A = Args.getLastArg(options::OPT_stdlib_EQ)) {
    StringRef Value = A->getValue();
    if (Value == "libc++")
      return ToolChain::CST_Libcxx;
    else if (Value == "libstdc++")
      return ToolChain::CST_Libstdcxx;
    else
      getDriver().Diag(diag::err_drv_invalid_stdlib_name)
          << A->getAsString(Args);
  }
  return ToolChain::CST_Libcxx;
}

void EVM::AddClangSystemIncludeArgs(const ArgList &DriverArgs,
                                            ArgStringList &CC1Args) const {
  if (DriverArgs.hasArg(clang::driver::options::OPT_nostdinc))
    return;

  const Driver &D = getDriver();

  if (!DriverArgs.hasArg(options::OPT_nobuiltininc)) {
    SmallString<128> P(D.ResourceDir);
    llvm::sys::path::append(P, "include");
    addSystemInclude(DriverArgs, CC1Args, P);
  }

  if (DriverArgs.hasArg(options::OPT_nostdlibinc))
    return;

  // Check for configure-time C include directories.
  StringRef CIncludeDirs(C_INCLUDE_DIRS);
  if (CIncludeDirs != "") {
    SmallVector<StringRef, 5> dirs;
    CIncludeDirs.split(dirs, ":");
    for (StringRef dir : dirs) {
      StringRef Prefix =
          llvm::sys::path::is_absolute(dir) ? "" : StringRef(D.SysRoot);
      addExternCSystemInclude(DriverArgs, CC1Args, Prefix + dir);
    }
    return;
  }

  if (getTriple().getOS() != llvm::Triple::UnknownOS) {
    const std::string MultiarchTriple =
        getMultiarchTriple(D, getTriple(), D.SysRoot);
    addSystemInclude(DriverArgs, CC1Args, D.SysRoot + "/include/" + MultiarchTriple);
  }
  addSystemInclude(DriverArgs, CC1Args, D.SysRoot + "/include");
}

void EVM::AddClangCXXStdlibIncludeArgs(const ArgList &DriverArgs,
                                               ArgStringList &CC1Args) const {

  if (DriverArgs.hasArg(options::OPT_nostdlibinc) ||
      DriverArgs.hasArg(options::OPT_nostdincxx))
    return;

  switch (GetCXXStdlibType(DriverArgs)) {
  case ToolChain::CST_Libcxx:
    addLibCxxIncludePaths(DriverArgs, CC1Args);
    break;
  case ToolChain::CST_Libstdcxx:
    addLibStdCXXIncludePaths(DriverArgs, CC1Args);
    break;
  }
}

void EVM::AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
                                      llvm::opt::ArgStringList &CmdArgs) const {

  switch (GetCXXStdlibType(Args)) {
  case ToolChain::CST_Libcxx:
    CmdArgs.push_back("-lc++");
    CmdArgs.push_back("-lc++abi");
    break;
  case ToolChain::CST_Libstdcxx:
    CmdArgs.push_back("-lstdc++");
    break;
  }
}

SanitizerMask EVM::getSupportedSanitizers() const {
  SanitizerMask Res = ToolChain::getSupportedSanitizers();
  if (getTriple().isOSEmscripten()) {
    Res |= SanitizerKind::Vptr | SanitizerKind::Leak | SanitizerKind::Address;
  }
  return Res;
}

Tool *EVM::buildLinker() const {
  return new tools::evm::Linker(*this);
}

void EVM::addLibCxxIncludePaths(
    const llvm::opt::ArgList &DriverArgs,
    llvm::opt::ArgStringList &CC1Args) const {
  const Driver &D = getDriver();
  std::string SysRoot = computeSysRoot();
  std::string LibPath = SysRoot + "/include";
  const std::string MultiarchTriple =
      getMultiarchTriple(D, getTriple(), SysRoot);
  bool IsKnownOs = (getTriple().getOS() != llvm::Triple::UnknownOS);

  std::string Version = detectLibcxxVersion(LibPath);
  if (Version.empty())
    return;

  // First add the per-target include path if the OS is known.
  if (IsKnownOs) {
    std::string TargetDir = LibPath + "/" + MultiarchTriple + "/c++/" + Version;
    addSystemInclude(DriverArgs, CC1Args, TargetDir);
  }

  // Second add the generic one.
  addSystemInclude(DriverArgs, CC1Args, LibPath + "/c++/" + Version);
}

void EVM::addLibStdCXXIncludePaths(
    const llvm::opt::ArgList &DriverArgs,
    llvm::opt::ArgStringList &CC1Args) const {
  // We cannot use GCCInstallationDetector here as the sysroot usually does
  // not contain a full GCC installation.
  // Instead, we search the given sysroot for /usr/include/xx, similar
  // to how we do it for libc++.
  const Driver &D = getDriver();
  std::string SysRoot = computeSysRoot();
  std::string LibPath = SysRoot + "/include";
  const std::string MultiarchTriple =
      getMultiarchTriple(D, getTriple(), SysRoot);
  bool IsKnownOs = (getTriple().getOS() != llvm::Triple::UnknownOS);

  // This is similar to detectLibcxxVersion()
  std::string Version;
  {
    std::error_code EC;
    Generic_GCC::GCCVersion MaxVersion =
        Generic_GCC::GCCVersion::Parse("0.0.0");
    SmallString<128> Path(LibPath);
    llvm::sys::path::append(Path, "c++");
    for (llvm::vfs::directory_iterator LI = getVFS().dir_begin(Path, EC), LE;
         !EC && LI != LE; LI = LI.increment(EC)) {
      StringRef VersionText = llvm::sys::path::filename(LI->path());
      if (VersionText[0] != 'v') {
        auto Version = Generic_GCC::GCCVersion::Parse(VersionText);
        if (Version > MaxVersion)
          MaxVersion = Version;
      }
    }
    if (MaxVersion.Major > 0)
      Version = MaxVersion.Text;
  }

  if (Version.empty())
    return;

  // First add the per-target include path if the OS is known.
  if (IsKnownOs) {
    std::string TargetDir = LibPath + "/c++/" + Version + "/" + MultiarchTriple;
    addSystemInclude(DriverArgs, CC1Args, TargetDir);
  }

  // Second add the generic one.
  addSystemInclude(DriverArgs, CC1Args, LibPath + "/c++/" + Version);
  // Third the backward one.
  addSystemInclude(DriverArgs, CC1Args, LibPath + "/c++/" + Version + "/backward");
}
