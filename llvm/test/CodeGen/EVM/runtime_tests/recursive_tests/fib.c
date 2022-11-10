// EVM_RUN: function: fib, input: [0], result: 0
// EVM_RUN: function: fib, input: [2], result: 1
// EVM_RUN: function: fib, input: [10], result: 55

int fib(int n)
{
  if (n <= 1)
    return n;
  return fib(n - 1) + fib(n - 2);
}
