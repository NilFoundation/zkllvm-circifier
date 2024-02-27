#include <cassert>

// There was a bug with the `main` function. For some reason, it was handled in
// a special way, although it shouldn't differ from the regular functions.
// EVM_RUN: function: main, input: [], result: 123
[[evm]] int main() {
    return 123;;
}

// EVM_RUN: function: add, input: [1,2], result: 3
// EVM_RUN: function: add, input: [0x100,0x23], result: 0x123
[[evm]]
int add(int a, int b) {
  return a + b;
}

// EVM_RUN: function: call_test, input: [], result: 10
int __attribute__ ((noinline)) callee(int a, int b) {
  return a - b;
}

[[evm]]
int call_test() {
  return 1 + callee(12, 3);
}

// EVM_RUN: function: test_same_args, input: [], result: 0
[[evm]]
int test_same_args() {
  return callee(1, 1);
}

__uint256_t v256 = 0x12345678123456781234567812345678ababababcdcdcdcdefefefef12345678X;

// EVM_RUN: function: test_casts, input: [], result: 0
[[evm]] int test_casts() {
  using u64 = unsigned long long;
  __uint128_t v128 = static_cast<__uint128_t>(v256);
  u64 v64 = static_cast<u64>(v256);
  unsigned v32 = static_cast<unsigned>(v256);
  unsigned short v16 = static_cast<unsigned short>(v256);
  unsigned char v8 = static_cast<unsigned char>(v256);

  if (v128 != 0xababababcdcdcdcdefefefef12345678X)
    return 1;
  if (v64 != 0xefefefef12345678)
    return 2;
  if (v32 != 0x12345678)
    return 3;
  if (v16 != 0x5678)
    return 4;
  if (v8 != 0x78)
    return 5;
  return 0;
}

// EVM_RUN: function: return_void, input: [1]
int return_void_value;

[[evm]] void return_void(int N) {
  return_void_value = N;
}

//! DEPLOY
//! CALL function: :test, expect_fail: true
[[evm]] void test() {
  assert(false);
}
