//===- IntrinsicsEVM.td - Defines EVM intrinsics -----------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the EVM-specific intrinsics.
//
//===----------------------------------------------------------------------===//

// Specialized loads from packet
let TargetPrefix = "evm" in {  // All intrinsics start with "llvm.evm."
  def int_evm_stop : ClangBuiltin<"__builtin_evm_stop">,
              Intrinsic<[], [], [IntrNoMem]>;
  def int_evm_mload : ClangBuiltin<"__builtin_evm_mload">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty], [IntrReadMem]>;
  def int_evm_mstore : ClangBuiltin<"__builtin_evm_mstore">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty], [IntrWriteMem]>;
  def int_evm_mstore8 : ClangBuiltin<"__builtin_evm_mstore8">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty], [IntrWriteMem]>;

  // The following are EVM-specific operations
  def int_evm_exp : ClangBuiltin<"__builtin_evm_exp">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty, llvm_i256_ty], [IntrNoMem]>;
  def int_evm_byte : ClangBuiltin<"__builtin_evm_byte">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty, llvm_i256_ty], [IntrNoMem]>;
  def int_evm_shl : ClangBuiltin<"__builtin_evm_shl">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty, llvm_i256_ty], [IntrNoMem]>;
  def int_evm_shr : ClangBuiltin<"__builtin_evm_shr">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty, llvm_i256_ty], [IntrNoMem]>;
  def int_evm_sar : ClangBuiltin<"__builtin_evm_sar">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty, llvm_i256_ty], [IntrNoMem]>;
  
  def int_evm_sha3 : ClangBuiltin<"__builtin_evm_sha3">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty, llvm_i256_ty], [IntrReadMem]>;
  def int_evm_address : ClangBuiltin<"__builtin_evm_address">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_balance : ClangBuiltin<"__builtin_evm_balance">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty], [IntrNoMem]>;
  def int_evm_origin : ClangBuiltin<"__builtin_evm_origin">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_caller : ClangBuiltin<"__builtin_evm_caller">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_callvalue : ClangBuiltin<"__builtin_evm_callvalue">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_calldataload : ClangBuiltin<"__builtin_evm_calldataload">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty], [IntrNoMem]>;
  def int_evm_calldatasize : ClangBuiltin<"__builtin_evm_calldatasize">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_calldatacopy : ClangBuiltin<"__builtin_evm_calldatacopy">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty], [IntrWriteMem]>;
  def int_evm_codesize : ClangBuiltin<"__builtin_evm_codesize">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_codecopy : ClangBuiltin<"__builtin_evm_codecopy">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty], [IntrWriteMem]>;
  def int_evm_gasprice : ClangBuiltin<"__builtin_evm_gasprice">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_extcodesize : ClangBuiltin<"__builtin_evm_extcodesize">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty], [IntrNoMem]>;
  def int_evm_extcodecopy : ClangBuiltin<"__builtin_evm_extcodecopy">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty, llvm_i256_ty], [IntrWriteMem]>;
  def int_evm_returndatasize : ClangBuiltin<"__builtin_evm_returndatasize">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_returndatacopy : ClangBuiltin<"__builtin_evm_returndatacopy">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty], [IntrWriteMem]>;

  // Those are getter intrinsics
  def int_evm_blockhash : ClangBuiltin<"__builtin_evm_blockhash">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty], [IntrNoMem]>;
  def int_evm_coinbase : ClangBuiltin<"__builtin_evm_coinbase">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_timestamp : ClangBuiltin<"__builtin_evm_timestamp">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_number : ClangBuiltin<"__builtin_evm_number">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_difficulty : ClangBuiltin<"__builtin_evm_difficulty">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_gaslimit : ClangBuiltin<"__builtin_evm_gaslimit">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;


  def int_evm_sload : ClangBuiltin<"__builtin_evm_sload">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty], [IntrNoMem, IntrHasSideEffects]>;
  def int_evm_sstore : ClangBuiltin<"__builtin_evm_sstore">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty], [IntrNoMem, IntrHasSideEffects]>;
  
  def int_evm_jump : ClangBuiltin<"__builtin_evm_jump">,
              Intrinsic<[], [llvm_i256_ty], [IntrNoMem]>;
  def int_evm_jumpi : ClangBuiltin<"__builtin_evm_jumpi">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty], [IntrNoMem]>;


  def int_evm_getpc : ClangBuiltin<"__builtin_evm_getpc">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_msize : ClangBuiltin<"__builtin_evm_msize">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;
  def int_evm_gas : ClangBuiltin<"__builtin_evm_gas">,
              Intrinsic<[llvm_i256_ty], [], [IntrNoMem]>;

  def int_evm_jumpdest : ClangBuiltin<"__builtin_evm_jumpdest">,
              Intrinsic<[], [], [IntrNoMem]>;
  

  def int_evm_log0 : ClangBuiltin<"__builtin_evm_log0">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty],
              [IntrNoMem, IntrHasSideEffects]>;
  def int_evm_log1 : ClangBuiltin<"__builtin_evm_log1">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty],
              [IntrNoMem, IntrHasSideEffects]>;
  def int_evm_log2 : ClangBuiltin<"__builtin_evm_log2">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                             llvm_i256_ty], [IntrNoMem, IntrHasSideEffects]>;
  def int_evm_log3 : ClangBuiltin<"__builtin_evm_log3">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                             llvm_i256_ty, llvm_i256_ty], [IntrNoMem, IntrHasSideEffects]>;
  def int_evm_log4 : ClangBuiltin<"__builtin_evm_log4">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                             llvm_i256_ty, llvm_i256_ty, llvm_i256_ty],
                            [IntrNoMem, IntrHasSideEffects]>;

  def int_evm_create : ClangBuiltin<"__builtin_evm_create">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty],
                        [IntrWriteMem, IntrHasSideEffects]>;

  def int_evm_call : ClangBuiltin<"__builtin_evm_call">,
              Intrinsic<[llvm_i256_ty],
                        [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                         llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                         llvm_i256_ty],
                        [IntrHasSideEffects]>;

  def int_evm_callcode : ClangBuiltin<"__builtin_evm_callcode">,
              Intrinsic<[llvm_i256_ty],
                        [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                         llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                         llvm_i256_ty],
                        [IntrHasSideEffects]>;

  def int_evm_create2 : ClangBuiltin<"__builtin_evm_create2">,
              Intrinsic<[llvm_i256_ty], [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty, llvm_i256_ty],
                        [IntrWriteMem, IntrHasSideEffects]>;

  def int_evm_revert : ClangBuiltin<"__builtin_evm_revert">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty], [IntrWriteMem, IntrHasSideEffects, IntrNoReturn]>;

  def int_evm_return : ClangBuiltin<"__builtin_evm_return">,
              Intrinsic<[], [llvm_i256_ty, llvm_i256_ty], [IntrWriteMem, IntrHasSideEffects, IntrNoReturn]>;

  def int_evm_staticcall : ClangBuiltin<"__builtin_evm_staticcall">,
              Intrinsic<[llvm_i256_ty],
                        [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                         llvm_i256_ty, llvm_i256_ty, llvm_i256_ty],
                        [IntrWriteMem, IntrHasSideEffects]>;
  def int_evm_delegatecall : ClangBuiltin<"__builtin_evm_delegatecall">,
              Intrinsic<[llvm_i256_ty],
                        [llvm_i256_ty, llvm_i256_ty, llvm_i256_ty,
                         llvm_i256_ty, llvm_i256_ty, llvm_i256_ty],
                        [IntrWriteMem, IntrHasSideEffects]>;
  def int_evm_invalid : ClangBuiltin<"__builtin_evm_invalid">,
              Intrinsic<[], [], [IntrNoMem]>;
  def int_evm_selfdestruct : ClangBuiltin<"__builtin_evm_selfdestruct">,
              Intrinsic<[], [llvm_i256_ty], [IntrWriteMem, IntrHasSideEffects]>;
}
