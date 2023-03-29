#include <cstring>
#include "evm_sdk.h"

// EVM_RUN: function: test_memset, input: [0xab], result: 0
[[evm]] int test_memset(int a) {
  __uint256_t val;
  memset(&val, 0xab, sizeof(val));
  if (val != 0xababababababababababababababababababababababababababababababababX)
    return 1;
  return 0;
}

// EVM_RUN: function: test_memcpy, input: [123], result: 0
[[evm]] int test_memcpy(int a) {
  static constexpr unsigned count = 7;
  int src[count];
  int dst[count];
  static_assert(sizeof(src) == count * 32);

  for (int i = 0; i < count; i++) {
    src[i] = a + i;
    dst[i] = -1;
  }
  memcpy(dst, src, sizeof(src));
  for (int i = 0; i < count; i++) {
    if (dst[i] != a + i)
      return 1;
  }
  return 0;
}

// EVM_RUN: function: test_struct_copy, input: [], result: 0

struct Foo {
  int n{1};
  char c{2};
};

[[evm]] int test_struct_copy(int n) {
  Foo f1{0x123, 45};
  Foo f2;
  if (f2.n != 1 || f2.c != 2)
    return 1;
  memcpy(&f2, &f1, sizeof(Foo));
  if (f2.n != 0x123 || f2.c != 45)
    return 2;
  return 0;
}

// EVM_RUN: function: modadd, input: [10, 5, 4], result: 3
// EVM_RUN: function: modadd, input: [10, 5, 5], result: 0
// EVM_RUN: function: modadd, input: [123456781234567812345678, 12345678, 1000], result: 356
[[evm]] int modadd(__uint256_t a, __uint256_t b, __uint256_t mod) {
  return __evm_builtin_modadd(a, b, mod);
}

// EVM_RUN: function: modmul, input: [10, 5, 4], result: 2
// EVM_RUN: function: modmul, input: [10, 5, 5], result: 0
// EVM_RUN: function: modmul, input: [123456781234567812345678, 12345678, 1000], result: 684
[[evm]] int modmul(__uint256_t a, __uint256_t b, __uint256_t mod) {
  return __evm_builtin_modmul(a, b, mod);
}

// Modulus is from bn128 field
// EVM_RUN: function: modpow, input: [0x1234567, 0x567, 0x30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd47], result: 0x23b9b2d58a15e5a74c5977ff07846ffd9939fcd5f70362383cdb260ece2f7182
[[evm]] int modpow(__uint256_t a, __uint256_t b, __uint256_t mod) {
  return __evm_builtin_modpow(a, b, mod);
}

