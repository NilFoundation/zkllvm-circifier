// EVM_RUN: function: ackermann, input: [0, 0], result: 1
// EVM_RUN: function: ackermann, input: [3, 2], result: 0x1d

int ackermann(int m, int n)
{
  if (!m) return n + 1;
  if (!n) return ackermann(m - 1, 1);
  return ackermann(m - 1, ackermann(m, n - 1));
}

