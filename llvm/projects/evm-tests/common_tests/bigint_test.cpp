using uint256_t = __uint256_t;

uint256_t add(uint256_t a, uint256_t b) {
  return a + b;
}

uint256_t sub(uint256_t a, uint256_t b) {
  return a - b;
}

// EVM_RUN: function: inverse, input: [0], result: 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
// EVM_RUN: function: inverse, input: [0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1], result: 0xe
[[evm]] uint256_t inverse(uint256_t a) {
  return ~a;
}

// EVM_RUN: function: test, input: [0, 0x1234567812345678123456781234567812345678123456781234567812345678, 1], result: 0x1234567812345678123456781234567812345678123456781234567812345679
// EVM_RUN: function: test, input: [0, 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff, 2], result: 1
// EVM_RUN: function: test, input: [1, 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff, 0xf], result: 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff0
// EVM_RUN: function: test, input: [1, 0x1, 2], result: 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
[[evm]] int test(int action, uint256_t a, uint256_t b) {
  switch(action) {
  case 0: return add(a, b);
  case 1: return sub(a, b);
  }
}
