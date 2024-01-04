
#include "evm_linker.h"
#include "llvm/Support/CommandLine.h"

static llvm::cl::opt<bool> OptVerbose("verbose",
                                      llvm::cl::desc("Verbose output"),
                                llvm::cl::init(false));

int main(int argc, char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv, "Link EVM compiled files\n");
  Logger::Enabled = OptVerbose;
  return EvmLinker().run();
}
