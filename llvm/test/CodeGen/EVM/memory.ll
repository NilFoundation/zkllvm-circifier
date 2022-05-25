; RUN: llvm-as < %s | llc -march=evm -filetype=asm | FileCheck %s


