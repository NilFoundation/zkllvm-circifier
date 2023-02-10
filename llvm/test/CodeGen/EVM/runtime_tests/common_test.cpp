// EVM_RUN: function: add, input: [1,2], result: 3
// EVM_RUN: function: add, input: [0x100,0x23], result: 0x123

[[evm]]
int add(int a, int b) {
  return a + b;
}

// EVM_RUN: function: call_test, input: [], result: 10
int callee(int a, int b) {
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
