// Test global pointers to symbols
// EVM_RUN: function: test, input: [12345], result: 12346

unsigned Value;
unsigned* Ptr = &Value;

[[evm]] int test(int n) {
  *Ptr = n;
  Value++;
  return Value;
}
