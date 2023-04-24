//===- IntrinsicsAssigner.td - Defines assigner intrinsics -----------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the ZKLLVM assigner-specific intrinsics.
//
//===----------------------------------------------------------------------===//

def int_assigner_malloc:    Intrinsic<[llvm_anyptr_ty], [llvm_i64_ty]>;
def int_assigner_free:      Intrinsic<[], [llvm_anyptr_ty]>;
def int_assigner_poseidon:  Intrinsic<[llvm_anyvector_ty], [LLVMMatchType<0>]>;
def int_assigner_sha2_256:  Intrinsic<[llvm_anyvector_ty], [LLVMMatchType<0>, LLVMMatchType<0>]>;
def int_assigner_sha2_512:  Intrinsic<[llvm_anyvector_ty], [LLVMMatchType<0>, LLVMMatchType<0>]>;
def int_assigner_optimal_ate_pairing: Intrinsic<[llvm_anyvector_ty], [llvm_any_ty, llvm_anyvector_ty]>;
