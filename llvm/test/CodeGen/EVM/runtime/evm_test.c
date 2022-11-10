// EVM_RUN: function: add, input: [1,2], result: 3
// EVM_RUN: function: add, input: [0x100,0x23], result: 0x123

int add(int a, int b) {
  return a + b;
}
