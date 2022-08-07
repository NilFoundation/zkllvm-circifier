// RUN: %clang -x c -triple evm -dwarf-version=4 -fsyntax-only -verify %s

struct {
  char f1[100];
  int f2;
} tmp = {};

unsigned invalid1(void) { return __builtin_evm_keccak(1, tmp); } // expected-error {{__builtin_evm_keccak argument 2 not a constant}}
unsigned invalid2(void) { return __builtin_evm_keccak(1, 1, 1); } // expected-error {{too many arguments to function call, expected 2, have 3}}

__evm_uint256_t valid1(void) { return __builtin_evm_keccak(tmp, &tmp.f2); }
