// EVM_RUN: function: test_function, input: [], result: 1

int unused_var = 1;
int used_var = 2;

int unused_function(int a) {
  return a + unused_var;
}

int used_function(int a) {
  return a - used_var;
}

[[evm]]
int test_function() {
  return used_function(3);
}
