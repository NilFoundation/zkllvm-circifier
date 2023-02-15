// EVM_RUN: function: test_ctz32, input: [0x173], result: 0
// EVM_RUN: function: test_ctz32, input: [0x17c], result: 2
// EVM_RUN: function: test_ctz32, input: [0x17c00], result: 10
// EVM_RUN: function: test_ctz32, input: [0x80000000], result: 31

[[evm]] int test_ctz32(unsigned long v) {
  return __builtin_ctz(v);
}

// EVM_RUN: function: test_ctz64, input: [0x173], result: 0
// EVM_RUN: function: test_ctz64, input: [0x17c00], result: 10
// EVM_RUN: function: test_ctz64, input: [0x80000000], result: 31
// EVM_RUN: function: test_ctz64, input: [0x800000000], result: 35
// EVM_RUN: function: test_ctz64, input: [0x2260000000000000], result: 53
// EVM_RUN: function: test_ctz64, input: [0x8000000000000000], result: 63

[[evm]] int test_ctz64(unsigned long long v) {
  return __builtin_ctzll(v);
}

// EVM_RUN: function: test_clz32, input: [0x1], result: 31
// EVM_RUN: function: test_clz32, input: [0x5], result: 29
// EVM_RUN: function: test_clz32, input: [0x100], result: 23
// EVM_RUN: function: test_clz32, input: [0x80000000], result: 0

[[evm]] int test_clz32(unsigned long v) {
  return __builtin_clz(v);
}

// EVM_RUN: function: test_clz64, input: [0x0000000000000001], result: 63
// EVM_RUN: function: test_clz64, input: [0x0000000000000121], result: 55
// EVM_RUN: function: test_clz64, input: [0x5000000000000000], result: 1
// EVM_RUN: function: test_clz64, input: [0x8000000000000000], result: 0
[[evm]] int test_clz64(unsigned long long v) {
  return __builtin_clzll(v);
}
