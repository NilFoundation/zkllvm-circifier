// EVM_RUN: function: add, input: [1,2], result: 3
// EVM_RUN: function: add, input: [0x100,0x23], result: 0x123

int add(int a, int b) {
  return a + b;
}

// EVM_RUN: function: call_test, input: [], result: 2
int callee() {
  return 1;
}

int call_test() {
  return 1 + callee();
}