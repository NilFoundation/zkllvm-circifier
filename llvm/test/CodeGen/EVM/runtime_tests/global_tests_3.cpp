// EVM_RUN: function: test, input: [123], result: 124

int add(int a, int b) {
  return a + b;
}
int (*Func)(int, int) = add;

[[evm]] int test(int n) {
  return Func(n, 1);
}