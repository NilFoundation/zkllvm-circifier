#include "evm_sdk.h"

// TODO: Enable these tests when initial elector will be committed to DBMS.
// This commit should fixed by removing `bytes.fromhex` from hex string.

//! DEPLOY
//! def calc(n); (0..n-1).reduce("") { |res, x| res += "%02X"%((x + 1) % 256) }; end
//! CALL function: :test_return_dyn, input: [0], no_result: true
//! CALL function: :test_return_dyn, input: [1], result: 1
//! CALL function: :test_return_dyn, input: [25], result: calc(25).to_i(16)
//! CALL function: :test_return_dyn, input: [500], result: calc(500)

[[evm]] void* test_return_dyn(unsigned size) {
  unsigned char buf[size];
  for (unsigned i = 0; i < size; i++) {
    buf[i] = i + 1;
  }
  return evm::return_data(buf, size);
}

//! def calc64(n); (0..n-1).reduce("") { |res, x| res += "%016X"%(x + 1) }; end
//! CALL function: :test_return_dyn_i64, input: [1], result: 1
//! CALL function: :test_return_dyn_i64, input: [25], result: calc64(25)
//! CALL function: :test_return_dyn_i64, input: [500], result: calc64(500)
[[evm]] void* test_return_dyn_i64(unsigned size) {
  uint64_t buf[size];
  for (unsigned i = 0; i < size; i++) {
    buf[i] = i + 1;
  }
  return evm::return_data(buf, size * sizeof(uint64_t));
}
