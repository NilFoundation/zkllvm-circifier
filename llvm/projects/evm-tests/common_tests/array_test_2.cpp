// Codegen use emitFill for last element in array ({0, 0}). In this test we
// check that EVM backend properly handles it.

// EVM_RUN: function: test, input: [], result: 0

unsigned wsize[2][2] = {{1, 2}, {0, 0}};

[[evm]] int test() {
  if (wsize[0][0] != 1 || wsize[0][1] != 2)
    return 1;
  if (wsize[1][0] != 0 || wsize[1][1] != 0)
    return 2;
  return 0;
}