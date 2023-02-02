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

def int_assigner_malloc: Intrinsic<[llvm_anyptr_ty], [llvm_i64_ty]>;
